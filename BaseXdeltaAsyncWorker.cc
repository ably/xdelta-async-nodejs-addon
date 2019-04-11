#include <napi.h>

#include <xdelta3.h>

#include <BaseXdeltaAsyncWorker.h>

using namespace Napi;

BaseXdeltaAsyncWorker::BaseXdeltaAsyncWorker(Buffer<uint8_t> &dictionary,
											Buffer<uint8_t> &input,
											Buffer<uint8_t> &result,
											Function& callback) : AsyncWorker(callback),
																	dictionary(ObjectReference::New(dictionary, 1)),
																	input(ObjectReference::New(input, 1)),
																	result(ObjectReference::New(result, 1)),
																	dictionaryPtr(dictionary.Data()),
																	inputPtr(input.Data()),
																	resultPtr(result.Data()),
																	dictionaryLength(dictionary.Length()),
																	inputLength(input.Length()),
																	resultLength(result.Length())
{
}

void BaseXdeltaAsyncWorker::OnOK()
{
    HandleScope scope(Env());

	if (res == 0)
	{
    	Callback().Call({Env().Undefined(), Number::New(Env(), bytesWritten)});
	}
	else
	{
		Object err = Object::New(Env());
		err.Set("code", res);
		const char *errorMessage = xd3_strerror(res);
		if (errorMessage != NULL)
		{
			err.Set("message", errorMessage);
		}
		else if (res == 28)
		{
			err.Set("message", "Insufficient output space.");
		}

    	Callback().Call({err});
	}

    dictionary.Unref();
    input.Unref();
    result.Unref();
}
