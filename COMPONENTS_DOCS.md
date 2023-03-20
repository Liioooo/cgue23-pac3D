# Available Components

## TransformComponent

| **Property** | **Description**                    | **Example** |
|--------------|------------------------------------|-------------|
| `position`   | Translation of the entity          | `0 0 0`     |
| `rotation`   | Local rotation of the entity (deg) | `0 0 0`     |
| `scale`      | Local scale of the entity          | `0 0 0`     |

## MeshRendererComponent

| **Property**      | **Description**                                                     | **Example**           |
|-------------------|---------------------------------------------------------------------|-----------------------|
| `asset-file`      | File to load 3D-Asset                                               | `model.fbx`           |
| `mesh`            | Built in mesh                                                       | `CG_SphereMesh_16_16` |
| `material`        | Material for this mesh. Overrides materials from asset files.       | `MyMaterial`          |
| `submesh-indices` | List of submeshes to render. Nothing means: everything is rendered. | `0,1,2`               |

**Built in Meshes:**

- CG_CubeMesh
- CG_SphereMesh
  - `CG_SphereMesh_N_M`: <br> 
    `N`: number of longitudinal Segments <br>
    `M`: number of latitudinal Segments
- CG_Capsule
  - `CG_Capsule_R_H`: <br>
    `R`: Radius <br>
    `H`: Height

## CameraComponent

| **Property** | **Description**                           | **Example**   |
|--------------|-------------------------------------------|---------------|
| `projection` | Projection: `perspective` or `orthogonal` | `perspective` |
| `near`       | Near plane                                | `0.1`         |
| `far`        | Far plane                                 | `100`         |
| `fov`        | Field of View (deg)                       | `60`          |
| `ortho-size` | Size of orthographic projection           | `10`          |
| `primary`    | Is this camera the primary?               | `true`        |

## ScriptComponent

| **Property** | **Description**                    | **Example**    |
|--------------|------------------------------------|----------------|
| `scriptName` | Name of the script to use          | `PlayerScript` |

Scripts must be registered in `Main.cpp` like this:
```c++
application->registerNativeScript<Game::CameraScript>("cameraScript");
```

## DirectionalLightComponent

| **Property**   | **Description**                                                             | **Example** |
|----------------|-----------------------------------------------------------------------------|-------------|
| `color`        | Color of the light                                                          | `1 1 1`     |
| `intensity`    | Intensity of the light                                                      | `1`         |
| `cast-shadows` | Should the light cast shadows? (Only one shadow casting light is supported) | `true`      |

## PointLightComponent

| **Property** | **Description**                                | **Example** |
|--------------|------------------------------------------------|-------------|
| `color`      | Color of the light                             | `1 1 1`     |
| `intensity`  | Intensity of the light                         | `1`         |
| `radius`     | Radius of influence for the light              | `5`         |
| `falloff`    | Falloff: 0 -> light falloff, 1 -> hard falloff | `1`         |

## SpotLightComponent

| **Property**  | **Description**                                | **Example** |
|---------------|------------------------------------------------|-------------|
| `color`       | Color of the light                             | `1 1 1`     |
| `intensity`   | Intensity of the light                         | `1`         |
| `radius`      | Radius of influence for the light              | `5`         |
| `falloff`     | Falloff: 0 -> light falloff, 1 -> hard falloff | `1`         |
| `inner-angle` | Inner cone angle (deg)                         | `50`        |
| `outer-angle` | Outer cone angle (deg)                         | `60`        |

## SkyboxComponent

| **Property**  | **Description**                                | **Example**  |
|---------------|------------------------------------------------|--------------|
| `hdri-path`   | Path to the environment map                    | `skybox.hdr` |
| `intensity`   | Intensity of the environment map               | `1`          |
| `lod`         | Level of detail to display                     | `1`          |

## RigidBodyComponent

| **Property**          | **Description**                                      | **Example** |
|-----------------------|------------------------------------------------------|-------------|
| `dynamic`             | Is the RigidBody dynamic                             | `true`      |
| `kinematic`           | Is the RigidBody kinematic                           | `false`     |
| `disable-gravity`     | Disable gravity                                      | `false`     |
| `mass`                | Mass of the RigidBody (kg)                           | `10`        |
| `linear-drag`         | Linear drag                                          | `0`         |
| `angular-drag`        | Angular drag                                         | `0`         |
| `collision-detection` | Collision detection type: `discrete` or `continuous` | `discrete`  |

## BoxColliderComponent

| **Property**          | **Description**           | **Example**   |
|-----------------------|---------------------------|---------------|
| `half-size`           | Half-size of the collider | `0.5 0.5 0.5` |
| `offset`              | Offset of the collider    | `0 0 0`       |
| `trigger`             | Is trigger                | `false`       |
| `material`            | Physics material to use   | `phyMaterial` |

## SphereColliderComponent

| **Property** | **Description**         | **Example**   |
|--------------|-------------------------|---------------|
| `radius`     | Radius of the collider  | `1`           |
| `offset`     | Offset of the collider  | `0 0 0`       |
| `trigger`    | Is trigger              | `false`       |
| `material`   | Physics material to use | `phyMaterial` |


