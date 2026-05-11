import unreal


CONTENT_FOLDERS = [
    "/Game/Blueprints/Vehicle",
    "/Game/Blueprints/Race",
    "/Game/Blueprints/Camera",
    "/Game/Blueprints/World",
    "/Game/Maps",
    "/Game/Vehicles",
    "/Game/Environment",
    "/Game/UI",
    "/Game/Audio",
    "/Game/Environment/Materials",
]


MATERIALS = {
    "MI_JCR_Asphalt": (0.055, 0.052, 0.047, 1.0, 0.0, 0.68),
    "MI_JCR_RiceField": (0.08, 0.26, 0.07, 1.0, 0.0, 0.86),
    "MI_JCR_Water": (0.035, 0.09, 0.085, 1.0, 0.0, 0.05),
    "MI_JCR_Shoulder": (0.31, 0.245, 0.155, 1.0, 0.0, 0.82),
    "MI_JCR_YellowLine": (1.0, 0.72, 0.055, 1.0, 0.0, 0.42),
    "MI_JCR_WhiteLine": (0.86, 0.88, 0.82, 1.0, 0.0, 0.38),
    "MI_JCR_Rail": (0.56, 0.56, 0.52, 1.0, 0.45, 0.32),
    "MI_JCR_HouseWall": (0.43, 0.38, 0.31, 1.0, 0.0, 0.72),
    "MI_JCR_Roof": (0.11, 0.105, 0.095, 1.0, 0.0, 0.82),
    "MI_JCR_Torii": (0.83, 0.055, 0.025, 1.0, 0.0, 0.48),
    "MI_JCR_Foliage": (0.025, 0.16, 0.035, 1.0, 0.0, 0.92),
    "MI_JCR_Rock": (0.22, 0.21, 0.19, 1.0, 0.0, 0.95),
    "MI_JCR_Mountain": (0.12, 0.155, 0.165, 1.0, 0.0, 0.96),
    "MI_JCR_DesertSand": (0.64, 0.49, 0.28, 1.0, 0.0, 0.78),
}


def ensure_folders():
    editor_asset_lib = unreal.EditorAssetLibrary
    for folder in CONTENT_FOLDERS:
        if not editor_asset_lib.does_directory_exist(folder):
            editor_asset_lib.make_directory(folder)


def create_materials():
    ensure_folders()
    asset_tools = unreal.AssetToolsHelpers.get_asset_tools()
    mat_path = "/Game/Environment/Materials"
    parent_path = "{}/M_JCR_MasterTint".format(mat_path)

    if not unreal.EditorAssetLibrary.does_asset_exist(parent_path):
        parent = asset_tools.create_asset(
            "M_JCR_MasterTint",
            mat_path,
            unreal.Material,
            unreal.MaterialFactoryNew(),
        )
        tint = unreal.MaterialEditingLibrary.create_material_expression(
            parent,
            unreal.MaterialExpressionVectorParameter,
            -360,
            -80,
        )
        tint.set_editor_property("parameter_name", "Tint")
        tint.set_editor_property("default_value", unreal.LinearColor(0.5, 0.5, 0.5, 1.0))
        roughness = unreal.MaterialEditingLibrary.create_material_expression(
            parent,
            unreal.MaterialExpressionScalarParameter,
            -360,
            120,
        )
        roughness.set_editor_property("parameter_name", "Roughness")
        roughness.set_editor_property("default_value", 0.75)
        metallic = unreal.MaterialEditingLibrary.create_material_expression(
            parent,
            unreal.MaterialExpressionScalarParameter,
            -360,
            220,
        )
        metallic.set_editor_property("parameter_name", "Metallic")
        metallic.set_editor_property("default_value", 0.0)
        unreal.MaterialEditingLibrary.connect_material_property(tint, "", unreal.MaterialProperty.MP_BASE_COLOR)
        unreal.MaterialEditingLibrary.connect_material_property(roughness, "", unreal.MaterialProperty.MP_ROUGHNESS)
        unreal.MaterialEditingLibrary.connect_material_property(metallic, "", unreal.MaterialProperty.MP_METALLIC)
        unreal.MaterialEditingLibrary.recompile_material(parent)
        unreal.EditorAssetLibrary.save_asset(parent_path)
    else:
        parent = unreal.load_asset(parent_path)

    for name, values in MATERIALS.items():
        asset_path = "{}/{}".format(mat_path, name)
        if unreal.EditorAssetLibrary.does_asset_exist(asset_path):
            inst = unreal.load_asset(asset_path)
        else:
            inst = asset_tools.create_asset(
                name,
                mat_path,
                unreal.MaterialInstanceConstant,
                unreal.MaterialInstanceConstantFactoryNew(),
            )
        unreal.MaterialEditingLibrary.set_material_instance_parent(inst, parent)
        unreal.MaterialEditingLibrary.set_material_instance_vector_parameter_value(
            inst,
            "Tint",
            unreal.LinearColor(values[0], values[1], values[2], values[3]),
        )
        unreal.MaterialEditingLibrary.set_material_instance_scalar_parameter_value(inst, "Metallic", values[4])
        unreal.MaterialEditingLibrary.set_material_instance_scalar_parameter_value(inst, "Roughness", values[5])
        unreal.EditorAssetLibrary.save_asset(asset_path)


def create_test_map():
    ensure_folders()
    create_materials()
    level_path = "/Game/Maps/M_JapanCountryRoad_Test"
    if unreal.EditorAssetLibrary.does_asset_exist(level_path):
        unreal.EditorLevelLibrary.load_level(level_path)
    else:
        unreal.EditorLevelLibrary.new_level(level_path)

    world = unreal.EditorLevelLibrary.get_editor_world()
    world_settings = world.get_world_settings()
    game_mode_class = unreal.load_class(None, "/Script/JCRRacerV4.JCRGameMode")
    if game_mode_class:
        world_settings.set_editor_property("default_game_mode", game_mode_class)

    builder_class = unreal.load_class(None, "/Script/JCRRacerV4.JCRWorldBuilder")
    if builder_class:
        builder = None
        for actor in unreal.EditorLevelLibrary.get_all_level_actors():
            if actor.get_class() == builder_class:
                builder = actor
                break
        if not builder:
            builder = unreal.EditorLevelLibrary.spawn_actor_from_class(
                builder_class,
                unreal.Vector(0.0, 0.0, 0.0),
                unreal.Rotator(0.0, 0.0, 0.0),
            )
        if builder:
            builder.set_actor_label("JCR_VisibleCourse")
            builder.set_editor_property("bSpawnCheckpointsAtRuntime", True)
            builder.set_editor_property("bBuildPreviewInEditor", True)

    race_manager_class = unreal.load_class(None, "/Script/JCRRacerV4.JCRRaceManager")
    if race_manager_class:
        manager = None
        for actor in unreal.EditorLevelLibrary.get_all_level_actors():
            if actor.get_class() == race_manager_class:
                manager = actor
                break
        if not manager:
            manager = unreal.EditorLevelLibrary.spawn_actor_from_class(
                race_manager_class,
                unreal.Vector(0.0, 0.0, 0.0),
                unreal.Rotator(0.0, 0.0, 0.0),
            )
        if manager:
            manager.set_actor_label("JCR_RaceManager")

    unreal.EditorLevelLibrary.save_current_level()
    unreal.log("Created clean JCR Racer V4 map at {}. Runtime course is spawned by JCRGameMode.".format(level_path))


if __name__ == "__main__":
    create_test_map()
