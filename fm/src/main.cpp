#include <iostream>
#include <memory>

#include "input.h"
#include "demodulation.h"
#include "output.h"
#include "receiver.h"


int main() {
    std::shared_ptr<input_wrapper> input = std::make_shared<input_wrapper>(input_wrapper());
    std::shared_ptr<demodulator> demod = std::make_shared<demodulator>(demodulator());
    std::shared_ptr<output_wrapper> output = std::make_shared<output_wrapper>(output_wrapper());

    receiver recv = receiver(input, demod, output);
	recv.run();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    recv.stop();

	return 0;
}
