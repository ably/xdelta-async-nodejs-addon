#ifndef BASE_XDELTA_ASYNC_WORKER_H
#define BASE_XDELTA_ASYNC_WORKER_H

#include <napi.h>

#include <xdelta3.h>

using namespace Napi;

class BaseXdeltaAsyncWorker : public AsyncWorker
{
	public:
		virtual void Execute() override = 0;
		void OnOK() override;

    protected:
        BaseXdeltaAsyncWorker(Buffer<uint8_t> &dictionary,
                              Buffer<uint8_t> &input,
                              Buffer<uint8_t> &result,
                              Function& callback);

        ObjectReference dictionary;
        ObjectReference input;
        ObjectReference result;
        uint8_t *dictionaryPtr;
        uint8_t *inputPtr;
        uint8_t *resultPtr;
        usize_t dictionaryLength;
        usize_t inputLength;
        usize_t resultLength;
        int res;
        usize_t bytesWritten;
};

#endif
