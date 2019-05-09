#include <map>

#include <napi.h>

#include <xdelta3.h>

#include <BaseXdeltaAsyncWorker.h>

using namespace std;
using namespace Napi;

class XdeltaEncodeAsyncWorker : public BaseXdeltaAsyncWorker
{
    public:
        const int64_t id;

        static string New(Buffer<uint8_t> &dictionary,
                        Buffer<uint8_t> &target,
                        Buffer<uint8_t> &result,
                        int32_t stringMatcherType,
                        Function& callback,
                        XdeltaEncodeAsyncWorker **encoder);

        static string RequestCancellation(int64_t id);

        void Execute() override;

    private:
        static map<int64_t, XdeltaEncodeAsyncWorker *> runningEncoders;
        uint8_t cancellationRequested;
        int32_t stringMatcherType;

        XdeltaEncodeAsyncWorker(int64_t id,
                                Buffer<uint8_t> &dictionary,
                                Buffer<uint8_t> &taget,
                                Buffer<uint8_t> &result,
                                int32_t stringMatcherType,
                                Function& callback);

        ~XdeltaEncodeAsyncWorker() override;

        static int64_t GetNewEncoderId();

        void RequestCancellation();
};
