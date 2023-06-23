#pragma once

namespace ve {

	class Player
	{
		int max_hp = 10;
		int hp = 10;
		int armor = 0;
		double attackSpeed = 1;
		double attackDmg = 1;
		int cost = 1;


		int resources = 0;


	public:
		Player() {};

		void addhp() {
			hp += 1;
			max_hp += 1;
		}

		void addArmor() {
			armor += 1;
		}

		void addAttackSpeed() {
			attackSpeed * 0.9;
		}

		void addAttackDmg() {
			attackDmg += 1;
		}

		void addResources() {
			resources += 1;
		}

		int getHp() {
			return hp;
		}
		int getArmor() {
			return armor;
		}

		double getAttackSpeed() {
			return attackSpeed;
		}

		double getAttackDamage() {
			return attackDmg;
		}

		int getResources() {
			return resources;
		}

		void getDamage(int dmg) {

			hp -= (dmg - armor);
		}

		bool pay() {
			if ((resources - cost) >= 0) {
				resources -= cost;
				cost++;

				return true;
			}
			return false;
		}

		void heal() {
			hp = max_hp;
		}

	};

}