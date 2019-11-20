.PHONY: build
build:
	@python setup.py build_ext --inplace

.PHONY: install
install:
	@pip install -e .

.PHONY: clean
clean:
	@rm -rf build dist filters.egg-info *.so filters/__pycache__ src/*.c
