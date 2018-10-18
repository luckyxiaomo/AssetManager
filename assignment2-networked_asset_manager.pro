TEMPLATE = subdirs

SUBDIRS += \
    qhttp-embed \
    asset_manager \
    asset_manager_tests

asset_manager.depends = qhttp-embed
