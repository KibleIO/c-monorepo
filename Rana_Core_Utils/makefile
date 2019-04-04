ALL:
	./make.sh

clean:
	./demake.sh
	rm *.so

push:
	./push_local
	./push_abroad

reload:
	make clean; make; make push
	cd ../Rana; make reload
	cd ../Iris; make reload
	cd ../Themis; make reload
