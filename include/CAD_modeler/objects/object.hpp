#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>


class Object {
public:
    Object();

protected:
    glm::vec3 position;
    glm::vec3 rotation;
    float scale;

    glm::mat4x4 model_matrix() const;

private:
    glm::mat4x4 scale_matrix() const;
    glm::mat4x4 rotation_matrix() const;
    glm::mat4x4 translation_matrix() const;

    glm::mat4x4 rotation_x_matrix() const;
    glm::mat4x4 rotation_y_matrix() const;
    glm::mat4x4 rotation_z_matrix() const;
};
