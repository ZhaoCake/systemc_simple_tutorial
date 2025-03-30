# SystemC项目总Makefile
SUBDIRS = mux_4to1 alu_4bit register_ram
BUILD_DIR = build

.PHONY: all clean $(SUBDIRS) prepare run $(patsubst %,run-%,$(SUBDIRS))

# 确保子目录目标依赖于prepare，确保构建目录已创建
all: prepare
	@for dir in $(SUBDIRS); do \
		echo "编译 $$dir"; \
		mkdir -p $(BUILD_DIR)/$$dir; \
		$(MAKE) -C $$dir BUILD_DIR=$(realpath $(BUILD_DIR))/$$dir || exit 1; \
	done

# 创建顶级构建目录
prepare:
	@mkdir -p $(BUILD_DIR)

# 运行所有测试
run: all
	@echo "====== 运行所有测试 ======"
	@for dir in $(SUBDIRS); do \
		echo "\n====== 运行 $$dir 测试 ======"; \
		$(MAKE) -C $$dir run BUILD_DIR=$(realpath $(BUILD_DIR))/$$dir || exit 1; \
	done
	@echo "\n====== 所有测试运行完成 ======"

# 单独运行特定模块的测试
$(patsubst %,run-%,$(SUBDIRS)): run-%: prepare
	@echo "====== 编译和运行 $* 测试 ======"
	@mkdir -p $(BUILD_DIR)/$*
	@$(MAKE) -C $* BUILD_DIR=$(realpath $(BUILD_DIR))/$*
	@$(MAKE) -C $* run BUILD_DIR=$(realpath $(BUILD_DIR))/$*

# 清理编译产物
clean:
	rm -rf $(BUILD_DIR)
	@echo "清理完成"
