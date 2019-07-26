#include <napi.h>

#include <xdelta3.h>

#include <XdeltaDecodeAsyncWorker.h>

using namespace Napi;

XdeltaDecodeAsyncWorker::XdeltaDecodeAsyncWorker(Buffer<uint8_t> &dictionary,
                                                Buffer<uint8_t> &delta,
                                                Buffer<uint8_t> &result,
                                                Function& callback) : BaseXdeltaAsyncWorker(dictionary,
                                                                                            delta,
                                                                                            result,
                                                                                            callback)
{
}

void XdeltaDecodeAsyncWorker::Execute()
{
    res = xd3_decode_memory(inputPtr,
                            inputLength,
                            dictionaryPtr,
                            dictionaryLength,
                            resultPtr,
                            &bytesWritten,
                            resultLength,
                            0);
}
