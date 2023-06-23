/**
* The Vienna Vulkan Engine
*
* (c) bei Helmut Hlavacs, University of Vienna
*
*/


#include "VEInclude.h"
#include "FFMPEGListener.h"
#include "GUIListener.h"
#include "Player.h"
#include "Projectile.h"
#include "Enemy.h"




namespace ve {


	float global_time = 0.0f;
	double g_time = 1.0;				//zeit die noch übrig ist
	bool g_gameLost = false;			//true... das Spiel wurde verloren
	bool g_restart = false; //true...das Spiel soll neu gestartet werden
	Player player;
	float level = 1;

	float levelduration = 15.0f;
	//
	//Zeichne das GUI
	//
	class EventListenerGUI : public VEEventListener {
	protected:

		virtual void onDrawOverlay(veEvent event) {
			VESubrender_Nuklear* pSubrender = (VESubrender_Nuklear*)getEnginePointer()->getRenderer()->getOverlay();
			if (pSubrender == nullptr) return;

			struct nk_context* ctx = pSubrender->getContext();

			if (global_time < levelduration && !g_gameLost) {
				if (nk_begin(ctx, "", nk_rect(0, 0, 200, 170), NK_WINDOW_BORDER)) {
					char outbuffer[100];
					nk_layout_row_dynamic(ctx, 45, 1);
					sprintf(outbuffer, "Resources: %03d", player.getResources());
					nk_label(ctx, outbuffer, NK_TEXT_LEFT);

					nk_layout_row_dynamic(ctx, 45, 1);
					sprintf(outbuffer, "Health: %d", (player.getHp()));
					nk_label(ctx, outbuffer, NK_TEXT_LEFT);
				}
				nk_end(ctx);
			}
			if (g_gameLost) {
				if (nk_begin(ctx, "", nk_rect(100, 100, 200, 170), NK_WINDOW_BORDER)) {
					nk_layout_row_dynamic(ctx, 45, 1);
					nk_label(ctx, "Game Over", NK_TEXT_LEFT);
					if (nk_button_label(ctx, "Restart")) {
						g_restart = true;
					}
				}
				nk_end(ctx);

			};


			if (global_time >= levelduration && !g_gameLost) {

				if (nk_begin(ctx, "", nk_rect(300, 200, 300, 300), NK_WINDOW_BORDER)) {
					char outbuffer[100];
					nk_layout_row_dynamic(ctx, 45, 1);
					sprintf(outbuffer, "Resources: %03d", player.getResources());
					nk_label(ctx, outbuffer, NK_TEXT_LEFT);

					nk_layout_row_dynamic(ctx, 45, 1);
					sprintf(outbuffer, "Health: %d", (player.getHp()));
					nk_label(ctx, outbuffer, NK_TEXT_LEFT);
					if (nk_button_label(ctx, "increase Health")) {
						if (player.pay()) {
							player.addhp();
						}
					}

					nk_layout_row_dynamic(ctx, 45, 1);
					sprintf(outbuffer, "AD: %.1f", (player.getAttackDamage()));
					nk_label(ctx, outbuffer, NK_TEXT_LEFT);
					if (nk_button_label(ctx, "increase AD")) {
						if (player.pay()) {
							player.addAttackDmg();
						}
					}

					nk_layout_row_dynamic(ctx, 45, 1);
					sprintf(outbuffer, "AS: %.1f", (player.getAttackSpeed()));
					nk_label(ctx, outbuffer, NK_TEXT_LEFT);
					if (nk_button_label(ctx, "increase AS")) {
						if (player.pay()) {
							player.addAttackSpeed();
						}
					}


					nk_layout_row_dynamic(ctx, 45, 1);
					if (nk_button_label(ctx, "Continue")) {
						global_time = 0.0f;
						level = level + 1;
					}
				}
				nk_end(ctx);
			}


		}

	public:
		///Constructor of class EventListenerGUI
		EventListenerGUI(std::string name) : VEEventListener(name) { };

		///Destructor of class EventListenerGUI
		virtual ~EventListenerGUI() {};
	};


	static std::default_random_engine e{ 1345 };					//Für Zufallszahlen
	static std::uniform_real_distribution<> d{ -10.0f, 10.0f };		//Für Zufallszahlen




	class EventListenerCollision : public VEEventListener {
		std::vector<Enemy*> enemies;
		std::vector<Projectile> projectiles;

		double timetoattack = player.getAttackSpeed();
		uint32_t mobcounter = 0;
		uint32_t projectilecounter = 0;
		float frame_time = 0.0f;

		std::string projToBeDeleted = "";
		std::string enemyToBeDeleted = "";



	protected:
		virtual void onFrameStarted(veEvent event) {
			static uint32_t cubeid = 0;

			if (global_time >= levelduration) {

				reset();
				getSceneManagerPointer()->getSceneNode("The Player Parent")->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));
				return;
			}

			if (g_restart) {
				g_gameLost = false;
				g_restart = false;
				player = Player();
				fullreset();
				getSceneManagerPointer()->getSceneNode("The Player Parent")->setPosition(glm::vec3(0.0f, 1.0f, 0.0f));



				return;
			}


			if (g_gameLost) return;

			glm::vec3 positionPlayer = getSceneManagerPointer()->getSceneNode("The Player Parent")->getPosition();

			frame_time = frame_time + event.dt;
			global_time = global_time + event.dt;

			getSceneManagerPointer()->getSceneNode("StandardCamera")->setPosition(positionPlayer);

			g_time -= event.dt;
			if (g_time <= 0) {
				spawnMob();
				g_time = 1.5;
			}

			timetoattack -= event.dt;
			if (timetoattack <= 0) {
				//std::cout << "shoot" << std::endl;
				shoot();
				timetoattack = player.getAttackSpeed();
			}

			int i = 0;
			for (auto projectile : projectiles) {
				VESceneNode& proj = *getSceneManagerPointer()->getSceneNode(projectile.getName());
				bool deleted = false;

				proj.setPosition(proj.getPosition() += projectile.getDirection() * float(20.0*event.dt));

				int j = 0;
				for (auto enemy : enemies) {

					//std::cout << "getpos for proj" << std::endl;
					auto positionEnemy = getSceneManagerPointer()->getSceneNode(enemy->getName())->getPosition();


					float distance = glm::length(positionEnemy - proj.getPosition());
					if (distance < 1) {
						//std::cout << "hit" << std::endl;
						enemy->getDamage(player.getAttackDamage());
						if (enemy->getHp() <= 0) {
							enemyToBeDeleted = enemy->getName();
						}
						projToBeDeleted = projectile.getName();
					}
					j++;
				}

				projectile.addTime(event.dt);
				if (projectile.getTimealive() > 1.0) {
					std::cout << "proj alive too long" << std::endl;
					projToBeDeleted = projectile.getName();
				}


				i++;
			}

			for (auto enemy : enemies) {

				//std::cout << "getpos for enemies" << std::endl;
				glm::vec3 posEnemy = getSceneManagerPointer()->getSceneNode(enemy->getName())->getPosition();
				//std::cout << "after getpos for enemies" << std::endl;


				moveToward(*getSceneManagerPointer()->getSceneNode(enemy->getName()), *getSceneManagerPointer()->getSceneNode("The Player Parent"), enemy->getSpeed() * event.dt);




				float distance = glm::length(posEnemy - positionPlayer);
				//std::cout << "after distcalc" << std::endl;
				if (distance < 1) {


					//std::cout << "getdamage" << std::endl;
					if (frame_time > 0.5) {
						player.getDamage(enemy->getAttackDamage());
						frame_time = 0.0f;
					}
					std::cout << enemy->getName() << std::endl;

					/*/getEnginePointer()->m_irrklangEngine->play2D("media/sounds/explosion.wav", false);
					if (g_score % 10 == 0) {
						//g_time = 30;
						getEnginePointer()->m_irrklangEngine->play2D("media/sounds/bell.wav", false);
					}*/
					if (player.getHp() <= 0) {
						g_gameLost = true;
					}

				}

				/*VESceneNode* eParent = getSceneManagerPointer()->getSceneNode("The Cube Parent");
				eParent->setPosition(glm::vec3(d(e), 1.0f, d(e)));

				getSceneManagerPointer()->deleteSceneNodeAndChildren("The Cube"+ std::to_string(cubeid));
				/*VECHECKPOINTER(getSceneManagerPointer()->loadModel("The Cube"+ std::to_string(++cubeid)  , "media/models/test/crate0", "cube.obj", 0, eParent) );*/
			}

			if (projToBeDeleted != "") {
				for (int i = 0; i < projectiles.size(); i++) {
					if (projectiles.at(i).getName() == projToBeDeleted) {
						projectiles.erase(projectiles.begin() + i);
					}
				}
				//std::cout << "delete proj" << std::endl;
				getSceneManagerPointer()->deleteSceneNodeAndChildren(projToBeDeleted);
				projToBeDeleted = "";
			}


			if (enemyToBeDeleted != "") {

				for (int i = 0; i < enemies.size(); i++) {

					if (enemies.at(i)->getName() == enemyToBeDeleted) {
						//std::cout << "delete enem" << std::endl;
						delete enemies.at(i);
						enemies.erase(enemies.begin() + i);
						player.addResources();
					}
				}
			
					getSceneManagerPointer()->deleteSceneNodeAndChildren(enemyToBeDeleted);
				    enemyToBeDeleted = "";	

					
				
			}


			//g_time -= event.dt;
			//if (g_time <= 0) {
				//g_gameLost = true;
				//getEnginePointer()->m_irrklangEngine->removeAllSoundSources();
				//getEnginePointer()->m_irrklangEngine->play2D("media/sounds/gameover.wav", false);
			//}
		};

		void spawnMob() {
			mobcounter++;
			std::string enemyName = "The Cube Parent" + std::to_string(mobcounter);
			//std::cout << enemyName << std::endl;

			Enemy* newmob = new Enemy(2*int(level), 1, enemyName);
			enemies.push_back(newmob);

			auto* pScene = getSceneManagerPointer()->getSceneNode("Level 1");
			VESceneNode* e1, * eParent;
			eParent = getSceneManagerPointer()->createSceneNode(enemyName, pScene, glm::mat4(1.0));
			VECHECKPOINTER(e1 = getSceneManagerPointer()->loadModel("The Cube" + std::to_string(mobcounter), "media/models/test/crate0", "cube.obj"));

			//eParent->multiplyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 3.0f)));
			eParent->addChild(e1);


			getSceneManagerPointer()->getSceneNode("The Cube Parent" + std::to_string(mobcounter))->setPosition(glm::vec3(d(e), 1.0f, d(e)));


		}

		void moveToward(VESceneNode& object, VESceneNode& target, float speed) {
			glm::vec3 direction = glm::normalize(target.getPosition() - object.getPosition());
			object.setPosition(object.getPosition() += (direction *level) * speed);
		}

		void shoot() {
			glm::vec3 positionPlayer = getSceneManagerPointer()->getSceneNode("The Player Parent")->getPosition();


			glm::vec3 positionEnemy = glm::vec3(1.0f);
			glm::vec3 closestEnemy = glm::vec3(1.0f);

			float minDistance = 1000.0f;
			for (auto enemy : enemies) {
				positionEnemy = getSceneManagerPointer()->getSceneNode(enemy->getName())->getPosition();
				float distance = glm::length(positionEnemy - positionPlayer);
				if (distance < minDistance) {
					minDistance = distance;
					closestEnemy = positionEnemy;
					std::cout << minDistance << " to " << enemy->getName() << std::endl;
				}
			}
			glm::vec3 direction = glm::normalize(closestEnemy - positionPlayer);


			projectilecounter++;
			std::string projectileName = "The Projetile Parent" + std::to_string(projectilecounter);
			projectiles.push_back(Projectile(direction, projectileName));



			auto* pScene = getSceneManagerPointer()->getSceneNode("Level 1");

			VESceneNode* projectileParent, * projectile;
			projectileParent = getSceneManagerPointer()->createSceneNode(projectileName, pScene, glm::mat4(1.0));
			VECHECKPOINTER(projectile = getSceneManagerPointer()->loadModel("The Projectile" + std::to_string(projectilecounter), "media/models/standard", "cube.obj"));

			projectileParent->multiplyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 1.0f)));
			projectile->setTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.15f, 0.15f, 0.15f)));
			projectileParent->addChild(projectile);
			projectileParent->setPosition(positionPlayer);


		}

	public:
		///Constructor of class EventListenerCollision
		EventListenerCollision(std::string name) : VEEventListener(name) { };

		///Destructor of class EventListenerCollision
		virtual ~EventListenerCollision() {};

		void reset() {

			for (int i = 0; i < projectiles.size(); i++) {
				getSceneManagerPointer()->deleteSceneNodeAndChildren(projectiles.at(i).getName());
				
				projectiles.erase(projectiles.begin() + i);
			}


			for (int i = 0; i < enemies.size(); i++) {
				getSceneManagerPointer()->deleteSceneNodeAndChildren(enemies.at(i)->getName());
				delete enemies.at(i);
				enemies.erase(enemies.begin() + i);
			}

			player.heal();
		}

		void fullreset() {

			for (int i = 0; i < projectiles.size(); i++) {
				getSceneManagerPointer()->deleteSceneNodeAndChildren(projectiles.at(i).getName());
				projectiles.erase(projectiles.begin() + i);
			}


			for (int i = 0; i < enemies.size(); i++) {
				getSceneManagerPointer()->deleteSceneNodeAndChildren(enemies.at(i)->getName());
				delete enemies.at(i);
				enemies.erase(enemies.begin() + i);
			}

			level = 1;
			global_time = 0.0f;
			frame_time = 0.0f;
		}


	};



	///user defined manager class, derived from VEEngine
	class MyVulkanEngine : public VEEngine {
	public:

		MyVulkanEngine(veRendererType type = veRendererType::VE_RENDERER_TYPE_FORWARD, bool debug = false) : VEEngine(type, debug) {};
		~MyVulkanEngine() {};


		///Register an event listener to interact with the user

		virtual void registerEventListeners() {

			VEEngine::registerEventListeners();

			registerEventListener(new EventListenerCollision("Collision"), { veEvent::VE_EVENT_FRAME_STARTED });
			registerEventListener(new EventListenerGUI("GUI"), { veEvent::VE_EVENT_DRAW_OVERLAY });
			registerEventListener(new FFMPEGListener("MyListener"), {veEvent::VE_EVENT_FRAME_ENDED});
			//registerEventListener(new GUIListener("MyGUIListener"), { veEvent::VE_EVENT_KEYBOARD, veEvent::VE_EVENT_DRAW_OVERLAY });


		};


		///Load the first level into the game engine
		///The engine uses Y-UP, Left-handed
		virtual void loadLevel(uint32_t numLevel = 1) {

			VEEngine::loadLevel(numLevel);			//create standard cameras and lights

			VESceneNode* pScene;
			VECHECKPOINTER(pScene = getSceneManagerPointer()->createSceneNode("Level 1", getRoot()));

			//scene models

			/*VESceneNode* sp1;
			VECHECKPOINTER( sp1 = getSceneManagerPointer()->createSkybox("The Sky", "media/models/test/sky/cloudy",
										{	"bluecloud_ft.jpg", "bluecloud_bk.jpg", "bluecloud_up.jpg",
											"bluecloud_dn.jpg", "bluecloud_rt.jpg", "bluecloud_lf.jpg" }, pScene)  );*/

			VESceneNode* e4;
			VECHECKPOINTER(e4 = getSceneManagerPointer()->loadModel("The Plane", "media/models/test/plane", "plane_t_n_s.obj", 0, pScene));
			e4->setTransform(glm::scale(glm::mat4(1.0f), glm::vec3(1000.0f, 1.0f, 1000.0f)));

			VEEntity* pE4;
			VECHECKPOINTER(pE4 = (VEEntity*)getSceneManagerPointer()->getSceneNode("The Plane/plane_t_n_s.obj/plane/Entity_0"));
			pE4->setParam(glm::vec4(1000.0f, 1000.0f, 0.0f, 0.0f));

			/*VESceneNode* e1, * eParent;
			eParent = getSceneManagerPointer()->createSceneNode("The Cube Parent", pScene, glm::mat4(1.0));
			VECHECKPOINTER(e1 = getSceneManagerPointer()->loadModel("The Cube0", "media/models/test/crate0", "cube.obj"));

			eParent->multiplyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 3.0f)));
			eParent->addChild(e1);*/


			VESceneNode* playerParent, * player;

			playerParent = getSceneManagerPointer()->createSceneNode("The Player Parent", pScene, glm::mat4(1.0));
			VECHECKPOINTER(player = getSceneManagerPointer()->loadModel("The Player", "media/models/test/sphere", "sphere.obj"));

			playerParent->multiplyTransform(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
			playerParent->addChild(player);
			player->setTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.01f, 0.01f, 0.01f)));




			//m_irrklangEngine->play2D("media/sounds/ophelia.wav", true);
		};
	};


}

using namespace ve;

int main() {
	bool debug = true;

	MyVulkanEngine mve(veRendererType::VE_RENDERER_TYPE_FORWARD, debug);	//enable or disable debugging (=callback, validation layers)

	mve.initEngine();
	mve.loadLevel(1);
	mve.run();

	return 0;
}
