#!/bin/bash

# Ejecutar matrix_eficiencia con los argumentos proporcionados
./matrix_eficiencia "$1" "$2"

# Ejecutar threads_eficiencia con los argumentos proporcionados
./threads_eficiencia "$1" "$2"

# Ejecutar fork_eficiencia con los argumentos proporcionados
./fork_eficiencia "$1" "$2"

# Ejecutar mmomp con los argumentos proporcionados
./mmomp "$1" "$2"
