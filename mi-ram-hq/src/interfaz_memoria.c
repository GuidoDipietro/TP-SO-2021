#include "../include/interfaz_memoria.h"

extern t_log* logger;
extern t_config_mrhq* cfg;

#define INICIO_INVALIDO (cfg->TAMANIO_MEMORIA+69)

bool iniciar_patota_en_mp(char* tareas) {
    // TODO: Contemplar paginacion
    bool segmentacion = strcmp(cfg->ESQUEMA_MEMORIA, "SEGMENTACION") == 0;

    static uint32_t PID = 1;

    if (segmentacion) {

        // Meto el segmento TAREAS
        uint32_t inicio_tareas = meter_segmento_en_mp((void*) tareas, strlen(tareas)+1);
        if (inicio_tareas == INICIO_INVALIDO) {
            log_error(logger, "Error catastrofico iniciando patota en MP");
            return false;
        }

        // Genero PCB y meto el segmento PCB
        void* pcb = malloc(2 * sizeof(uint32_t));
        uint32_t dl_tareas = 1; // SIEMPRE es el segundo segmento

        memcpy(pcb, &PID, sizeof(uint32_t)); PID++;
        memcpy(pcb+sizeof(uint32_t), &dl_tareas, sizeof(uint32_t));

        uint32_t inicio_pcb = meter_segmento_en_mp(pcb, 2*sizeof(uint32_t));
        if (inicio_pcb == INICIO_INVALIDO) {
            log_error(logger, "Error catastrofico iniciando patota en MP");
            return false;
        }
        free(pcb);

        // Creo tabla y actualizo tabla de patotas
        segmento_t* seg_pcb = new_segmento(0, inicio_pcb, 8);
        segmento_t* seg_tareas = new_segmento(1, inicio_tareas, strlen(tareas)+1);
        ts_patota_t* tabla = malloc(sizeof(ts_patota_t));
        tabla->pcb = seg_pcb;
        tabla->tareas = seg_tareas;
        tabla->pid = PID-1;

        list_add_tspatotas(tabla);
    }

    else {
        // TODO Paginacion
    }

    return true;
}

bool iniciar_tripulante_en_mp(uint32_t n_tripulantes, t_list* posiciones) {

}