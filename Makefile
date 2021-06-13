build:
	make build -C discordiador
	make build -C i_mongo_store
	make build -C mi-ram-hq

debug:
	make debug -C discordiador
	make debug -C i_mongo_store
	make debug -C mi-ram-hq

clean:
	make clean -C shared
	make clean -C discordiador
	make clean -C i_mongo_store
	make clean -C mi-ram-hq
	make clean -C tests

# Esto tambien borra todos los archivos de log
recontra:
	make clean
	find . -name "*.log" -type f -delete