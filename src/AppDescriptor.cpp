#include <esp_app_desc.h>
#include <esp_idf_version.h>
#include <sdkconfig.h>

#define INKPOINTX_STRING_IMPL(value) #value
#define INKPOINTX_STRING(value) INKPOINTX_STRING_IMPL(value)

// Arduino ships a weak descriptor in its prebuilt ESP-IDF archive. Its project
// version can describe the machine that built that archive, not this firmware.
// Bind the bootloader/OTA descriptor to the same version the UI reports.
extern "C" const __attribute__((section(".rodata_desc"), used)) esp_app_desc_t esp_app_desc = {
    .magic_word = ESP_APP_DESC_MAGIC_WORD,
#ifdef CONFIG_BOOTLOADER_APP_SECURE_VERSION
    .secure_version = CONFIG_BOOTLOADER_APP_SECURE_VERSION,
#endif
    .version = CROSSPOINT_VERSION,
    .project_name = "InkPointX",
    .time = __TIME__,
    .date = __DATE__,
    .idf_ver = INKPOINTX_STRING(ESP_IDF_VERSION_MAJOR) "." INKPOINTX_STRING(ESP_IDF_VERSION_MINOR) "." INKPOINTX_STRING(
        ESP_IDF_VERSION_PATCH),
    .min_efuse_blk_rev_full = CONFIG_ESP_EFUSE_BLOCK_REV_MIN_FULL,
    .max_efuse_blk_rev_full = CONFIG_ESP_EFUSE_BLOCK_REV_MAX_FULL,
    .mmu_page_size = 31 - __builtin_clz(CONFIG_MMU_PAGE_SIZE),
};

static_assert(sizeof(CROSSPOINT_VERSION) <= sizeof(esp_app_desc.version));
