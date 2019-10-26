#include <iostream>
#include <memory>

#include "in/rtl_source.h"
#include "blocks/decimation.h"
#include "blocks/fm_demodulation.h"
#include "blocks/downsampling.h"
#include "out/file_sink.h"
#include "receiver.h"


int main() {
    std::shared_ptr<source> input = std::shared_ptr<source>(
        new rtl_source(0));
    std::shared_ptr<flow> decimator = std::shared_ptr<flow>(
        new decimator(10));
    std::shared_ptr<flow> fm_demodulator = std::shared_ptr<flow>(
        new fm_demodulator());
    std::shared_ptr<flow> downsampler = std::shared_ptr<flow>(
        new downsampler(5));
    std::shared_ptr<sink> output = std::shared_ptr<sink>(
        new file_sink("audio"));

    receiver recv = receiver(input, decimator, fm_demodulator, downsampler, output);
	recv.run();

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    recv.stop();

	return 0;
}
