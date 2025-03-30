# SystemC项目总Makefile
SUBDIRS = mux_4to1
BUILD_DIR = build

.PHONY: all clean $(SUBDIRS) prepare run $(patsubst %,run-%,$(SUBDIRS))

all: prepare $(SUBDIRS)

prepare:
	mkdir -p $(BUILD_DIR)

# 编译子目录
$(SUBDIRS):
	$(MAKE) -C $@ BUILD_DIR=$(realpath $(BUILD_DIR))/$@

# 运行所有测试
run: prepare $(SUBDIRS)
	@echo "====== 运行所有测试 ======"
	@for dir in $(SUBDIRS); do \
		echo "\n====== 运行 $$dir 测试 ======"; \
		$(MAKE) -C $$dir run BUILD_DIR=$(realpath $(BUILD_DIR))/$$dir || exit 1; \
	done
	@echo "\n====== 所有测试运行完成 ======"

# 单独运行特定模块的测试
$(patsubst %,run-%,$(SUBDIRS)): prepare
	@echo "====== 运行 $(@:run-%=%) 测试 ======"
	@$(MAKE) -C $(@:run-%=%) run BUILD_DIR=$(realpath $(BUILD_DIR))/$(@:run-%=%)

# 清理编译产物
clean:
	rm -rf $(BUILD_DIR)
	@echo "清理完成"
