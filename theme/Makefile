.PHONY: clean all
all: $(patsubst %.png,%.bin,$(wildcard *.png))
%.bin: %.png
	@convert -rotate 90 $< bgr:$@;
clean:
	@rm -rf *.bin
