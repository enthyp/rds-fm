.PHONY: build
build:
	@python setup.py build_ext --inplace

.PHONY: install
install:
	@pip install -e .

.PHONY: clean
clean:
	@rm -rf build dist dsp.egg-info *.so dsp/__pycache__ src/*.c src/*.html
