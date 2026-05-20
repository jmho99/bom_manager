cmake_minimum_required(VERSION 3.16)

project(inventory_bom_manager VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

qt_add_executable(inventory_bom_manager
    main.cpp

    src/app/AppContext.h
    src/app/AppContext.cpp

    src/core/models/Product.h
    src/core/models/Material.h
    src/core/models/BomItem.h
    src/core/models/InventoryEvent.h
    src/core/models/StockSummary.h

    src/core/services/ProductService.h
    src/core/services/ProductService.cpp
    src/core/services/MaterialService.h
    src/core/services/MaterialService.cpp
    src/core/services/BomService.h
    src/core/services/BomService.cpp
    src/core/services/InventoryService.h
    src/core/services/InventoryService.cpp
    src/core/services/LogImportExportService.h
    src/core/services/LogImportExportService.cpp

    src/data/DatabaseManager.h
    src/data/DatabaseManager.cpp
    src/data/Schema.h
    src/data/Schema.cpp
    src/data/repositories/ProductRepository.h
    src/data/repositories/ProductRepository.cpp
    src/data/repositories/MaterialRepository.h
    src/data/repositories/MaterialRepository.cpp
    src/data/repositories/BomRepository.h
    src/data/repositories/BomRepository.cpp
    src/data/repositories/InventoryEventRepository.h
    src/data/repositories/InventoryEventRepository.cpp

    src/ui/MainWindow.h
    src/ui/MainWindow.cpp
    src/ui/product/ProductPage.h
    src/ui/product/ProductPage.cpp
    src/ui/product/ProductDialog.h
    src/ui/product/ProductDialog.cpp
    src/ui/material/MaterialPage.h
    src/ui/material/MaterialPage.cpp
    src/ui/material/MaterialDialog.h
    src/ui/material/MaterialDialog.cpp
    src/ui/bom/BomPage.h
    src/ui/bom/BomPage.cpp
    src/ui/bom/BomItemDialog.h
    src/ui/bom/BomItemDialog.cpp
    src/ui/event/EventLogPage.h
    src/ui/event/EventLogPage.cpp
    src/ui/common/StockEventDialog.h
    src/ui/common/StockEventDialog.cpp
    src/ui/common/BuildDialog.h
    src/ui/common/BuildDialog.cpp

    src/utils/DateTimeUtils.h
    src/utils/DateTimeUtils.cpp
    src/utils/MessageBoxHelper.h
    src/utils/MessageBoxHelper.cpp
)

target_include_directories(inventory_bom_manager PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(inventory_bom_manager PRIVATE
    Qt6::Widgets
    Qt6::Sql
)

if(WIN32)
    set_target_properties(inventory_bom_manager PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
