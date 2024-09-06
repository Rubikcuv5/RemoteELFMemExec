#include <stdlib.h>
#include <curl/curl.h>
#include "download.h"
#include "debug.h"
#include "util.h"




void download(const char* url, struct MemoryStruct *mem) {
    DEBUG_PRINT("URL: %s \n",url);
    CURL *curl;
    CURLcode res;

    // Inicializar libcurl
    res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res != CURLE_OK) {
        DEBUG_PRINT("curl_global_init() failed: %s\n", curl_easy_strerror(res));
        exit(EXIT_FAILURE);
    }

    // Inicializar una sesión de curl
    curl = curl_easy_init();
    if(!curl) {
        DEBUG_PRINT("curl_easy_init() failed\n");
        curl_global_cleanup();
        exit(EXIT_FAILURE);
    }

    // Establecer opciones de curl
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_memory);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)mem);
    curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L); // Para que curl falle en códigos de error HTTP

    // Realizar la solicitud
    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        if (res == CURLE_COULDNT_RESOLVE_HOST || res == CURLE_COULDNT_CONNECT || res == CURLE_OPERATION_TIMEDOUT) {
            DEBUG_PRINT("Network error: %s\n", curl_easy_strerror(res));
        } else if (res == CURLE_HTTP_RETURNED_ERROR) {
            long http_code = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            DEBUG_PRINT("HTTP error: %ld\n", http_code);
        } else {
            DEBUG_PRINT("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        exit(EXIT_FAILURE);
    } else {
        DEBUG_PRINT("Download successful\n");
    }

    // Limpiar recursos de curl
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

int is_url_active(const char *url) {
    CURL *curl;
    CURLcode res;
    long http_code = 0;
    int active = 0;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); // Solo verificar la existencia
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // Tiempo de espera de 10 segundos
        
        // Configurar una función de callback para mostrar información de depuración
        //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);  // Habilitar la salida detallada
        res = curl_easy_perform(curl);
        
        if (res == CURLE_OK) {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            if (http_code >= 200 && http_code < 300) {
                active = 1; // URL activa
            } else {
                DEBUG_PRINT("HTTP code %ld para la URL: %s\n", http_code, url);
            }
        } else {
            DEBUG_PRINT( "CURL error: %s\n", curl_easy_strerror(res));
        }
        
        curl_easy_cleanup(curl);
    } else {
        DEBUG_PRINT("CURL initialization failed\n");
    }

    return active;
}