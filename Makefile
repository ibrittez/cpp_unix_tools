.PHONY: host arm64 clean help

# Colores para output
GREEN  := \033[0;32m
YELLOW := \033[0;33m
NC     := \033[0m

# Detectar número de cores
NPROC := $(shell nproc 2>/dev/null || echo 4)

SSH_TARGET ?= root@192.168.4.1
DEPLOY_PATH ?= /root

help:
	@echo "$(GREEN)Available targets:$(NC)"
	@echo "  $(YELLOW)make host$(NC)   		- Compile for host (x86_64)"
	@echo "  $(YELLOW)make arm64$(NC)  		- Cross-compile for ARM64 (Linaro)"
	@echo "  $(YELLOW)make all$(NC)    		- Compile both targets"
	@echo "  $(YELLOW)make clean$(NC)  		- Clean all build artifacts"
	@echo "  $(YELLOW)make deploy$(NC) 		- Copy ARM64 binaries to target device"

# Compilar para host
host:
	@echo "$(GREEN)Building for HOST (x86_64)...$(NC)"
	@mkdir -p build/host
	@cd build/host && cmake ../.. && make -j$(NPROC)
	@echo "$(GREEN)✓ Host binaries in build/host/bin/$(NC)"

# Cross-compilar para ARM64
arm64:
	@echo "$(GREEN)Cross-compiling for ARM64 (Linaro)...$(NC)"
	@mkdir -p build/arm64
	@cd build/arm64 && cmake -DCMAKE_TOOLCHAIN_FILE=../../toolchain-linaro.cmake ../.. && make -j$(NPROC)
	@echo "$(GREEN)✓ ARM64 binaries in build/arm64/bin/$(NC)"

# Compilar ambos
all: host arm64
	@echo "$(GREEN)✓ All targets built$(NC)"

# Limpiar todo
clean:
	@echo "$(YELLOW)Cleaning build artifacts...$(NC)"
	@rm -rf build
	@echo "$(GREEN)✓ Clean complete$(NC)"

deploy: arm64
	@echo "$(GREEN)Deploying to $(SSH_TARGET):$(DEPLOY_PATH)$(NC)"
	@ssh $(SSH_TARGET) "mkdir -p $(DEPLOY_PATH)"
	@scp build/arm64/bin/procesos_arm64 $(SSH_TARGET):$(DEPLOY_PATH)/procesos
