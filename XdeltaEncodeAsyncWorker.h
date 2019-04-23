#include <map>

#include <napi.h>

#include <xdelta3.h>

#include <BaseXdeltaAsyncWorker.h>

using namespace std;
using namespace Napi;

class XdeltaEncodeAsyncWorker : public BaseXdeltaAsyncWorker
{
    public:
        const uint16_t id;

        static string New(Buffer<uint8_t> &dictionary,
                        Buffer<uint8_t> &target,
                        Buffer<uint8_t> &result,
                        int32_t stringMatcherType,
                        Function& callback,
                        XdeltaEncodeAsyncWorker **encoder);

        static string RequestCancellation(int32_t id);

        static string SetMaxSimultaneouslyRunningEncoders(int32_t requestedMaxSimultaneouslyRunningEncoders);

        void Execute() override;

    private:
        static uint16_t maxSimultaneouslyRunningEncoders;
        static map<uint16_t, XdeltaEncodeAsyncWorker *> runningEncoders;
        uint8_t cancellationRequested;
        int32_t stringMatcherType;

        XdeltaEncodeAsyncWorker(uint16_t id,
                                Buffer<uint8_t> &dictionary,
                                Buffer<uint8_t> &taget,
                                Buffer<uint8_t> &result,
                                int32_t stringMatcherType,
                                Function& callback);

        ~XdeltaEncodeAsyncWorker() override;

        static uint16_t GetNewEncoderId();

        void RequestCancellation();
};
