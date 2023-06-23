#pragma once
namespace ve {
	class Enemy {
		int hp = 3;
		int attackdamage;
		std::string name;
		float speed = 0.5;

	public:
		Enemy() {};
		Enemy(int hp, int attackdamage, std::string name)
		{ 
			this->name = name;
			this->hp = hp;
			this->attackdamage = attackdamage;
		};

		int getHp() {
			return hp;
		}

		int getAttackDamage(){
			return attackdamage;
		}

		std::string getName() {
			return name;
		}

		float getSpeed() {
			return speed;
		}

		void getDamage(int dmg) {
			hp -= dmg;
			std::cout << "damage :" << dmg << std::endl;
		}


	};
}