#pragma once
#include <glm/detail/type_vec.hpp>
namespace ve {
	class Projectile {

		float timealive = 0.0f;
		glm::vec3 direction;
		std::string name;


	public:
		Projectile() {};
		Projectile(glm::vec3 direction, std::string name) {
			this->direction = direction;
			this->name = name;
		}

		glm::vec3 getDirection() {
			return direction;
		}

		std::string getName() {
			return name;
		}

		float getTimealive() {
			return timealive;
		}

		void addTime(float dt) {
			timealive += dt;
		}
		

	};
}