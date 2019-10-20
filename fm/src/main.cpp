#include <iostream>
#include <memory>

#include "input.h"
#include "demodulation.h"
#include "output.h"
#include "receiver.h"


int main() {
    std::shared_ptr<input_wrapper> input = std::shared_ptr<input_wrapper>(new input_wrapper(0));
    std::shared_ptr<demodulator> demod = std::shared_ptr<demodulator>(new demodulator());
    std::shared_ptr<output_wrapper> output = std::shared_ptr<output_wrapper>(new output_wrapper("bob"));

    receiver recv = receiver(input, demod, output);
	recv.run();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    recv.stop();

	return 0;
}
