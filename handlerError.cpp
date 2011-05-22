/*/
	ErrorHandler.cpp (14.01.2005)
/*/

#include "handlerError.h"

void trans_func( unsigned int u, _EXCEPTION_POINTERS* pExp )
{
	switch (u)
	{
	case STATUS_WAIT_0:
		throw STATUS_WAIT_0_Exception();
		break;  
	case STATUS_ABANDONED_WAIT_0:
		throw STATUS_ABANDONED_WAIT_0_Exception();
		break; 
	case STATUS_USER_APC:
		throw STATUS_USER_APC_Exception();
		break; 
	case STATUS_TIMEOUT:
		throw STATUS_TIMEOUT_Exception();
		break; 
	case STATUS_PENDING:
		throw STATUS_PENDING_Exception();
		break; 
	case STATUS_SEGMENT_NOTIFICATION:
		throw STATUS_SEGMENT_NOTIFICATION_Exception();
		break; 
	case STATUS_GUARD_PAGE_VIOLATION:
		throw STATUS_GUARD_PAGE_VIOLATION_Exception();
		break; 
	case STATUS_DATATYPE_MISALIGNMENT:
		throw STATUS_DATATYPE_MISALIGNMENT_Exception();
		break; 
	case STATUS_BREAKPOINT:
		throw STATUS_BREAKPOINT_Exception();
		break; 
	case STATUS_SINGLE_STEP:
		throw STATUS_SINGLE_STEP_Exception();
		break; 
	case STATUS_ACCESS_VIOLATION:
		throw SSTATUS_ACCESS_VIOLATION_Exception();
		break; 
	case STATUS_IN_PAGE_ERROR:
		throw STATUS_IN_PAGE_ERROR_Exception();
		break; 
	case STATUS_INVALID_HANDLE:
		throw STATUS_INVALID_HANDLE_Exception();
		break; 
	case STATUS_NO_MEMORY:
		throw STATUS_NO_MEMORY_Exception();
		break; 
	case STATUS_ILLEGAL_INSTRUCTION:
		throw STATUS_ILLEGAL_INSTRUCTION_Exception();
		break; 
	case STATUS_NONCONTINUABLE_EXCEPTION:
		throw STATUS_NONCONTINUABLE_EXCEPTION_Exception();
		break; 
	case STATUS_INVALID_DISPOSITION:
		throw STATUS_INVALID_DISPOSITION_Exception();
		break; 
	case STATUS_ARRAY_BOUNDS_EXCEEDED:
		throw STATUS_ARRAY_BOUNDS_EXCEEDED_Exception();
		break; 
	case STATUS_FLOAT_DENORMAL_OPERAND:
		throw STATUS_FLOAT_DENORMAL_OPERAND_Exception();
		break; 
	case STATUS_FLOAT_DIVIDE_BY_ZERO:
		throw STATUS_FLOAT_DIVIDE_BY_ZERO_Exception();
		break; 
	case STATUS_FLOAT_INEXACT_RESULT:
		throw STATUS_FLOAT_INEXACT_RESULT_Exception();
		break; 
	case STATUS_FLOAT_INVALID_OPERATION:
		throw STATUS_FLOAT_INVALID_OPERATION_Exception();
		break; 
	case STATUS_FLOAT_STACK_CHECK:
		throw STATUS_FLOAT_OVERFLOW_Exception();
		break; 
	case STATUS_FLOAT_UNDERFLOW:
		throw STATUS_FLOAT_UNDERFLOW_Exception();
		break; 
	case STATUS_INTEGER_DIVIDE_BY_ZERO:
		throw STATUS_INTEGER_DIVIDE_BY_ZERO_Exception();
		break; 
	case STATUS_INTEGER_OVERFLOW:
		throw STATUS_INTEGER_OVERFLOW_Exception();
		break; 
	case STATUS_PRIVILEGED_INSTRUCTION:
		throw STATUS_PRIVILEGED_INSTRUCTION_Exception();
		break; 
	case STATUS_STACK_OVERFLOW:
		throw STATUS_STACK_OVERFLOW_Exception();
		break;
	case STATUS_CONTROL_C_EXIT:
		throw STATUS_CONTROL_C_EXIT_Exception();
		break;
	default:
		throw STATUS_UNKNOWN_Exception();
		break; 
	}
}
