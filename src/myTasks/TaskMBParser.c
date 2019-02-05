/*
 * TaskMBParser.c
 *
 *  Created on: 25.01.2019
 *      Author: E6430
 */

#include "TaskMBParser.h"



void vTaskMBParser(void *pvParameters){
	// working frame
	sADUFrame aduframe;
	// assign input stream buffer
	sParserInOutParameter *params;
	params = (sParserInOutParameter*)pvParameters;
	StreamBufferHandle_t inputStreamHandle = params->inputStream;
	QueueHandle_t outputMessageQueueHandle = params->outputMessageQueue;
	uint8_t sourceSocket = params->sourceSocketNo;

	while(1){
		// start of frame - wait for MBAP (7bytes) + function code (1byte) = 8bytes
		while(xStreamBufferBytesAvailable(inputStreamHandle) < 8){vTaskDelay(pdMS_TO_TICKS(10));}

		uint8_t	temp[8];
		xStreamBufferReceive(inputStreamHandle, temp, 8, portMAX_DELAY);
		memcpy(&(aduframe),temp,8);		// bytes up to uint id
		// need to swap bytes in uint16's cause stm is holding them LSB first
		//aduframe.MBAP.TransID = BYTES_IN_WORD_SWAP(aduframe.MBAP.TransID);		// not really necessary, we don't deal with it, just resend it as it was
		//aduframe.MBAP.ProtocolID = BYTES_IN_WORD_SWAP(aduframe.MBAP.ProtocolID);	// not really necessary as always =0
		aduframe.MBAP.Length = BYTES_IN_WORD_SWAP(aduframe.MBAP.Length);
		aduframe.PDU.FunctionCode = (eFcnCode)temp[7];
		// check for ProtocolID = 0
		if(aduframe.MBAP.ProtocolID != 0){
			// there is some error in transmission or framing, so reset the connection
			sW5500ControlMessage reply;
			reply.MessageType = W5500_KILL_CONNECTION;
			reply.SocketNo = sourceSocket;
			reply.MessageLength = 0;
			reply.DataPointer = NULL;
			xQueueSend(outputMessageQueueHandle, &reply, pdMS_TO_TICKS(500));
		}

		// now ADU is filled up to function code and data byte count is known (=Length-1)
		uint16_t data_bytes_received = 0;
		while(data_bytes_received < aduframe.MBAP.Length - 2){
			data_bytes_received += xStreamBufferReceive(inputStreamHandle, &(aduframe.PDU.Data)+data_bytes_received, 10, pdMS_TO_TICKS(20));
		}
		// now aduframe should be complete

		// decide what function to perform
		switch(aduframe.PDU.FunctionCode){
			case FCN_READ_HOLD_REGS:
				vProcessReadHoldingRegs(&aduframe);
				break;
			case FCN_WRITE_SING_REG:
				vProcessWriteSingleHoldingReg(&aduframe);
				;
				break;
			default:
				;
				// respond with exception 01
				vProcessException(&aduframe, EXC_CODE_ILLEGAL_FCN);
				break;
		}
		// now aduframe contains a response
		// so put it into message
		sW5500ControlMessage reply;
		reply.MessageType = W5500_MESSAGE_SEND;
		reply.SocketNo = sourceSocket;
		reply.MessageLength = aduframe.MBAP.Length + 6;
		reply.DataPointer = &aduframe;
		// before sending, swap bytes in word fields
		aduframe.MBAP.Length = BYTES_IN_WORD_SWAP(aduframe.MBAP.Length);
		//aduframe.MBAP.TransID = BYTES_IN_WORD_SWAP(aduframe.MBAP.TransID);	// as before, do not deal with it, just resend as received
		xQueueSend(outputMessageQueueHandle, &reply, pdMS_TO_TICKS(500));
		// Is it safe to assume reply.DataPointer will hold a valid aduframe until that aduframe is send out to socket?
		// Assuming modbus client will not send another request before getting an answer to previous, no new data will come into
		// aduframe buffer before it will be sent out to socket. But to be sure no new data will overwrite aduframe, wait some
		// short time to give W5500 process some time to send data from message queue.
		vTaskDelay(pdMS_TO_TICKS(50));

	}
}

void vProcessReadHoldingRegs(sADUFrame *request_frame){
	uint16_t starting_reg = (((uint16_t)*(request_frame->PDU.Data))<<8) | ((uint16_t)*(request_frame->PDU.Data+1));
	uint16_t regs_count = (((uint16_t)*(request_frame->PDU.Data+2))<<8) | ((uint16_t)*(request_frame->PDU.Data+3));

	// check if registers quantity is ok
	if((regs_count>=HOLDING_REGS_COUNT)||regs_count<1||regs_count>125){
		// prepare exception 03
		vProcessException(request_frame, EXC_CODE_ILLEGAL_DATA_VAL);
	}else{
		// check if starting address ok
		if(starting_reg+regs_count >= HOLDING_REGS_COUNT){
			// prepare exception 02
			vProcessException(request_frame, EXC_CODE_ILLEGAL_DATA_ADDR);
		}else{
			// actual processing
			// lets use already alloctated adu frame, fill it up with response values and clear the rest of bytes
			// mbap.transid, protocolid, unitid, pdu.fcncode - left the same
			// mbap.length - calculate new! = 1byte unitid + 1byte fcncode + 1byte bytecount + 2xregscount
			request_frame->MBAP.Length = 3+2*regs_count;
			// start filing data
			// first byte after fcn code = byte count (see specs)
			request_frame->PDU.Data[0] = 2*regs_count;
			for(uint16_t reg_pointer=0; reg_pointer<regs_count; reg_pointer++){
				int16_t reg_val = uiGetRegisterValue(reg_pointer+starting_reg);
				request_frame->PDU.Data[1+2*reg_pointer] = (uint8_t)(reg_val>>8);
				request_frame->PDU.Data[2+2*reg_pointer] = (uint8_t)(reg_val&0x00FF);
			}
			//
			// now request_frame contains response, either actual response to request or an exception
		}
	}
}

void vProcessWriteSingleHoldingReg(sADUFrame *request_frame){
	uint16_t starting_reg = (((uint16_t)*(request_frame->PDU.Data))<<8) | ((uint16_t)*(request_frame->PDU.Data+1));
	uint16_t reg_value = (((uint16_t)*(request_frame->PDU.Data+2))<<8) | ((uint16_t)*(request_frame->PDU.Data+3));


	// check if address ok
	if(starting_reg>=HOLDING_REGS_COUNT){
		// prepare exception 02
		vProcessException(request_frame, EXC_CODE_ILLEGAL_DATA_ADDR);
	}else{
		// actual processing
		// lets use already alloctated adu frame, fill it up with response values and clear the rest of bytes
		// mbap.transid, protocolid, unitid, pdu.fcncode - left the same
		// mbap.length - calculate new! = 1byte unitid + 1byte fcncode + 2bytes reg address + 2bytes reg value
		request_frame->MBAP.Length = 6;
		// fill with data
		// first 2 bytes after fcn code = register address - leave it as already is
		// next 2 bytes = register value - leave as it is
		// do actual register write
		uiSetRegisterValue(starting_reg, reg_value);
		//
		// now request_frame contains response, either actual response to request or an exception
	}
}

void vProcessWriteHoldingRegs(sADUFrame *request_frame){
	uint16_t starting_reg = (((uint16_t)*(request_frame->PDU.Data))<<8) | ((uint16_t)*(request_frame->PDU.Data+1));
	uint16_t regs_count = (((uint16_t)*(request_frame->PDU.Data+2))<<8) | ((uint16_t)*(request_frame->PDU.Data+3));

	// check if registers quantity is ok
	if((regs_count>=HOLDING_REGS_COUNT)||regs_count<1||regs_count>125){
		// prepare exception 03
		vProcessException(request_frame, EXC_CODE_ILLEGAL_DATA_VAL);
	}else{
		// check if starting address ok
		if(starting_reg+regs_count >= HOLDING_REGS_COUNT){
			// prepare exception 02
			vProcessException(request_frame, EXC_CODE_ILLEGAL_DATA_ADDR);
		}else{
			// actual processing
			// lets use already alloctated adu frame, fill it up with response values and clear the rest of bytes
			// mbap.transid, protocolid, unitid, pdu.fcncode - left the same
			// mbap.length - calculate new! = 1byte unitid + 1byte fcncode + 2bytes starting reg + 2 bytes regs count
			request_frame->MBAP.Length = 6;
			// start filing data - starting from Data[6]
			for(uint16_t reg_pointer=0; reg_pointer<regs_count; reg_pointer++){
				uint8_t *pHiByte = request_frame->PDU.Data+2*reg_pointer+6;
				uint8_t *pLoByte = pHiByte+1;
				int16_t reg_value = (((uint16_t)*(pHiByte))<<8) | ((uint16_t)*(pLoByte));
				uiSetRegisterValue(reg_pointer+starting_reg, reg_value);
			}
			//
			// now request_frame contains response, either actual response to request or an exception
		}
	}
}

void vProcessException(sADUFrame *request_frame, uint8_t exception_code){
	// set MSB bit of function code
	request_frame->PDU.FunctionCode += 0x80;
	// calculate new mbap.length
	request_frame->MBAP.Length = 3;		// unitid, fcncode, exccode
	// set exception code
	request_frame->PDU.Data[0] = exception_code;
}

void DataModelInit(){
	// initialize registers with 0s
	memset(&DataModel.HoldingRegs, 0x00, HOLDING_REGS_COUNT);
}

int16_t uiGetRegisterValue(uint16_t register_no){
	return DataModel.HoldingRegs[register_no];
	// for testing
	//return register_no + 100;
	// release semaphore...
}

void uiSetRegisterValue(uint16_t register_no, int16_t value){
	// get semaphore...
	DataModel.HoldingRegs[register_no] = value;
	// release semaphore...
}
