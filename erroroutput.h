#pragma once

#define check_ret(ret, funcCall)\
	if(ret < 0){				\
	FunctionReturn(funcCall)	\
	}	

#define check_p(p, funCall) \
	if(!p){					\
	FunctionReturn(funCall)	\
	}
#define FunctionReturn(func){func;return false;}