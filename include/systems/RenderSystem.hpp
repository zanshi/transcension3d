//
// Created by Benjamin Wiberg on 2015-04-10.
//

#pragma once

#include "entityx/entityx.h"

#include "glm/glm.hpp"

#include "components/all_components.hpp"

#include "events/StartDimensionChangeEvent.hpp"
#include "events/DimensionChangeInProgressEvent.hpp"
#include "events/ProjectionViewEvent.hpp"

// for pow(float, float)
#include <math.h>

/* Forward declarations */
class ShaderProgram;

namespace ex = entityx;

namespace sw {

/** Placeholder rendering system that processes entities with the following components
 *      - TransformComponent
 *      - RenderComponent
 *
 *  It logs the BodyComponent's position and rotation vectors as well as the RenderComponent's debug string
 *  **/

    class RenderSystem : public ex::System<RenderSystem>, public ex::Receiver<RenderSystem> {
    public:
        RenderSystem(ex::EventManager &events);

        ~RenderSystem();

        void updateSceneLightsData(ex::EntityManager &es, ex::TimeDelta dt);

        void updatePlayerData(ex::EntityManager &es, ex::TimeDelta dt);

        void renderEntity(ex::ComponentHandle<DimensionComponent> dimension,
                          ex::ComponentHandle<PhysicsComponent> physics,
                          ex::ComponentHandle<MeshComponent> mesh,
                          ex::ComponentHandle<ShadingComponent> shading);

        void update(ex::EntityManager &es, ex::EventManager &events, ex::TimeDelta dt) override;

        // Adapted from: http://www.learnopengl.com/
        void initShader();

        void receive(const StartDimensionChangeEvent &startChange);

        float generateEaseScale(float time);

        void receive(const DimensionChangeInProgressEvent &dimChange);

    private:
        /* GAME-RELATED VARIABLES */
        Dim current_dim_;
        Dim dim_from_;
        bool dim_change_in_progress_;
        bool has_received_dimension_in_progress_event_ = false;
        glm::mat4 dim_change_scale_mat_;

        /* RENDER-RELATED VARIABLES */
        ShaderProgram *shader_;

        // Reference to the top node in the tree, where rendering ALWAYS starts
        // Projection matrix and view / camera / player matrix transform
        glm::mat4 camera_projection_;

        glm::mat4 view_;
        /* Lighting */
        static const int MAX_POINT_LIGHTS = 20, MAX_DIR_LIGHTS = 20;
        static const int POINT_LIGHT_ATTRS = 4, DIR_LIGHT_ATTRS = 5;

        // Keeps track of how many lights of each type there are currently
        int num_point_lights_, num_dir_lights_, num_spot_lights;

        // Uniform variables locations
        GLint pointLightsLoc[MAX_POINT_LIGHTS][POINT_LIGHT_ATTRS];
        GLint dirLightsLoc[MAX_DIR_LIGHTS][DIR_LIGHT_ATTRS];
        GLint num_point_lights_loc, num_dir_lights_loc;

        GLint viewPosLoc;
        //Material
        GLint matlAmbientLoc, matlDiffuseLoc, matlSpecularLoc, matlShineLoc;

        // Matrices
        GLint P_loc, V_loc, V_inv_loc, M_loc;

        // Temp variables, used (and reused) for GLM decomposition. Reused to remove need to redeclare them each time
        glm::vec3 temp3;
        glm::vec4 temp4;
        glm::quat tempquat;

        void combine(ex::ComponentHandle<TransformComponent> transform, ex::Entity parent_entity);



    };
}
