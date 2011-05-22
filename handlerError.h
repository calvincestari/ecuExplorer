/*/
	ErrorHandler.h (14.01.2005)
/*/
#pragma once

// includes
#include <wfc.h>
#include <eh.h>

// local event type definitions
#define EVENT_INFO								1
#define EVENT_WARNING							2
#define EVENT_ERROR								3

#define EVENT_LAYER_NONE						0x00
#define EVENT_LAYER_1							0x01
#define EVENT_LAYER_2							0x02
#define EVENT_LAYER_3							0x04
#define EVENT_LAYER_4							0x08
#define EVENT_LAYER_5							0x10
#define EVENT_LAYER_6							0x20
#define EVENT_LAYER_7							0x40
#define EVENT_LAYER_8							0x80

// function prototypes
void trans_func(unsigned,_EXCEPTION_POINTERS*);

enum
{
	EXCEPT_NONE,
	EXEPT_CONTINUE,
	EXEPT_ABORT
};

// exception handler classes
#define GenericEventHandlerClass(nn)\
	class nn \
	{\
	public:\
		nn(){}\
		nn(nn&){}\
		~nn(){}\
	};

GenericEventHandlerClass(STATUS_WAIT_0_Exception);
GenericEventHandlerClass(STATUS_ABANDONED_WAIT_0_Exception);  
GenericEventHandlerClass(STATUS_USER_APC_Exception);  
GenericEventHandlerClass(STATUS_TIMEOUT_Exception);  
GenericEventHandlerClass(STATUS_PENDING_Exception);   
GenericEventHandlerClass(STATUS_SEGMENT_NOTIFICATION_Exception);  
GenericEventHandlerClass(STATUS_GUARD_PAGE_VIOLATION_Exception);
GenericEventHandlerClass(STATUS_DATATYPE_MISALIGNMENT_Exception);  
GenericEventHandlerClass(STATUS_BREAKPOINT_Exception); 
GenericEventHandlerClass(STATUS_SINGLE_STEP_Exception);  
GenericEventHandlerClass(SSTATUS_ACCESS_VIOLATION_Exception);  
GenericEventHandlerClass(STATUS_IN_PAGE_ERROR_Exception);  
GenericEventHandlerClass(STATUS_INVALID_HANDLE_Exception);
GenericEventHandlerClass(STATUS_NO_MEMORY_Exception);  
GenericEventHandlerClass(STATUS_ILLEGAL_INSTRUCTION_Exception); 
GenericEventHandlerClass(STATUS_NONCONTINUABLE_EXCEPTION_Exception);   
GenericEventHandlerClass(STATUS_INVALID_DISPOSITION_Exception); 
GenericEventHandlerClass(STATUS_ARRAY_BOUNDS_EXCEEDED_Exception);
GenericEventHandlerClass(STATUS_FLOAT_DENORMAL_OPERAND_Exception);  
GenericEventHandlerClass(STATUS_FLOAT_DIVIDE_BY_ZERO_Exception);  
GenericEventHandlerClass(STATUS_FLOAT_INEXACT_RESULT_Exception);  
GenericEventHandlerClass(STATUS_FLOAT_INVALID_OPERATION_Exception);     
GenericEventHandlerClass(STATUS_FLOAT_OVERFLOW_Exception);  
GenericEventHandlerClass(STATUS_FLOAT_UNDERFLOW_Exception);  
GenericEventHandlerClass(STATUS_INTEGER_DIVIDE_BY_ZERO_Exception);
GenericEventHandlerClass(STATUS_INTEGER_OVERFLOW_Exception);   
GenericEventHandlerClass(STATUS_PRIVILEGED_INSTRUCTION_Exception);   
GenericEventHandlerClass(STATUS_STACK_OVERFLOW_Exception);  
GenericEventHandlerClass(STATUS_CONTROL_C_EXIT_Exception);
GenericEventHandlerClass(STATUS_UNKNOWN_Exception);

// CATCHCATCH routine, creates exception handling similar to that of VB
#define CATCHCATCH(name)\
    }\
    catch( STATUS_WAIT_0_Exception e ){\
		sError.Format("STATUS_WAIT_0_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_ABANDONED_WAIT_0_Exception e ){\
		sError.Format("STATUS_ABANDONED_WAIT_0_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_USER_APC_Exception e ){\
		sError.Format("STATUS_USER_APC_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_TIMEOUT_Exception e ){\
		sError.Format("STATUS_TIMEOUT_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_CONTINUE;}\
    catch( STATUS_PENDING_Exception e ){\
		sError.Format( "STATUS_PENDING_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_SEGMENT_NOTIFICATION_Exception e ){\
		sError.Format( "STATUS_SEGMENT_NOTIFICATION_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_GUARD_PAGE_VIOLATION_Exception e ){\
		sError.Format( "STATUS_GUARD_PAGE_VIOLATION_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_DATATYPE_MISALIGNMENT_Exception e ){\
		sError.Format( "STATUS_DATATYPE_MISALIGNMENT_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_BREAKPOINT_Exception e ){\
		sError.Format( "STATUS_BREAKPOINT_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_SINGLE_STEP_Exception e ){\
		sError.Format( "STATUS_SINGLE_STEP_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( SSTATUS_ACCESS_VIOLATION_Exception e ){\
		sError.Format( "SSTATUS_ACCESS_VIOLATION_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_CONTINUE;}\
    catch( STATUS_IN_PAGE_ERROR_Exception e ){\
		sError.Format( "STATUS_IN_PAGE_ERROR_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_INVALID_HANDLE_Exception e ){\
		sError.Format( "STATUS_INVALID_HANDLE_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_NO_MEMORY_Exception e ){\
		sError.Format( "STATUS_NO_MEMORY_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_CONTINUE;}\
    catch( STATUS_ILLEGAL_INSTRUCTION_Exception e ){\
		sError.Format( "STATUS_ILLEGAL_INSTRUCTION_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_NONCONTINUABLE_EXCEPTION_Exception e ){\
		sError.Format( "STATUS_NONCONTINUABLE_EXCEPTION_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_INVALID_DISPOSITION_Exception e ){\
		sError.Format( "STATUS_INVALID_DISPOSITION_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_ARRAY_BOUNDS_EXCEEDED_Exception e ){\
		sError.Format( "STATUS_ARRAY_BOUNDS_EXCEEDED_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_FLOAT_DENORMAL_OPERAND_Exception e ){\
		sError.Format( "STATUS_FLOAT_DENORMAL_OPERAND_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_FLOAT_DIVIDE_BY_ZERO_Exception e ){\
		sError.Format( "STATUS_FLOAT_DIVIDE_BY_ZERO_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_CONTINUE;}\
    catch( STATUS_FLOAT_INEXACT_RESULT_Exception e ){\
		sError.Format( "STATUS_FLOAT_INEXACT_RESULT_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_FLOAT_INVALID_OPERATION_Exception e ){\
		sError.Format( "STATUS_FLOAT_INVALID_OPERATION_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_FLOAT_OVERFLOW_Exception e ){\
		sError.Format( "STATUS_FLOAT_OVERFLOW_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_CONTINUE;}\
    catch( STATUS_FLOAT_UNDERFLOW_Exception e ){\
		sError.Format( "STATUS_FLOAT_UNDERFLOW_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_CONTINUE;}\
    catch( STATUS_INTEGER_DIVIDE_BY_ZERO_Exception e ){\
		sError.Format( "STATUS_INTEGER_DIVIDE_BY_ZERO_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_CONTINUE;}\
    catch( STATUS_INTEGER_OVERFLOW_Exception e ){\
		sError.Format( "STATUS_INTEGER_OVERFLOW_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_CONTINUE;}\
    catch( STATUS_PRIVILEGED_INSTRUCTION_Exception e ){\
		sError.Format( "STATUS_PRIVILEGED_INSTRUCTION_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_STACK_OVERFLOW_Exception e ){\
		sError.Format( "STATUS_STACK_OVERFLOW_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;}\
    catch( STATUS_CONTROL_C_EXIT_Exception e ){\
		sError.Format( "STATUS_CONTROL_C_EXIT_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_CONTINUE;}\
    catch( STATUS_UNKNOWN_Exception e ){\
		sError.Format( "STATUS_UNKNOWN_Exception in %s [%i]",name,iExceptionBookmark);\
		OutputString(sError,0);\
		bExceptionFlag = EXEPT_ABORT;\
}

#define EXCEPTION_BOOKMARK(position)\
	iExceptionBookmark = (int)position;\

// TRYTRY routine, creates exception handling similar to that of VB
#define TRYTRY\
	int bExceptionFlag = EXCEPT_NONE;\
	int iExceptionBookmark = 0;\
    _set_se_translator(trans_func);\
    try\
    {
