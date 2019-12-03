PACK_NAME=dsp

.PHONY: build
build:
	@python setup.py build_ext

.PHONY: install
install:
	@pip install -e .

.PHONY: clean
clean:
	@rm -rf build dist $(PACK_NAME).egg-info *.so $(PACK_NAME)/__pycache__ $(PACK_NAME)/*.so src/*.c src/*.html
