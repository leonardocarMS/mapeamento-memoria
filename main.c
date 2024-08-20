#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    // Definição do caminho do arquivo
    const char *filename = "./big-file.txt";

    // Abre o arquivo 'big-file.txt' em modo de leitura
    int fd = open(filename, O_RDONLY);
    if (fd == -1) { 
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);  
    }

    // Obtém as informações do arquivo, como seu tamanho
    struct stat sb;
    if (fstat(fd, &sb) == -1) { 
        perror("Erro ao obter o tamanho do arquivo");
        close(fd); 
        exit(EXIT_FAILURE);
    }

    // Obtém o tamanho da página de memória do sistema
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1) {  
        perror("Erro ao obter o tamanho da página");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Mapeia o arquivo na memória, permitindo leitura direta
    char *mapped = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) { 
        perror("Erro ao mapear o arquivo na memória");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Exibe o conteúdo do arquivo utilizando o mapeamento de memória
    printf("Conteúdo do arquivo:\n%.*s\n", (int)sb.st_size, mapped);

    // Calcula o número de páginas de memória usadas para armazenar o arquivo
    long num_pages = (sb.st_size + page_size - 1) / page_size;  
    printf("\nTamanho do arquivo: %lld bytes\n", (long long)sb.st_size);
    printf("Tamanho da página: %ld bytes\n", page_size);
    printf("Número de páginas usadas: %ld\n", num_pages);

    // Desfaz o mapeamento do arquivo da memória
    if (munmap(mapped, sb.st_size) == -1) {  
        perror("Erro ao desfazer o mapeamento de memória");
    }

    // Fecha o descritor de arquivo
    close(fd);

    return 0;  
}
