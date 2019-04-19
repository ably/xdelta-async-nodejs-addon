#include <napi.h>

#include <xdelta3.h>

#include <BaseXdeltaAsyncWorker.h>

using namespace Napi;

class XdeltaDecodeAsyncWorker : public BaseXdeltaAsyncWorker
{
    public:
        XdeltaDecodeAsyncWorker(Buffer<uint8_t> &dictionary,
                                Buffer<uint8_t> &delta,
                                Buffer<uint8_t> &result,
                                Function& callback);

        void Execute() override;
};
