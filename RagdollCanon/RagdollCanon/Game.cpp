#include "Game.h"
#include "Ragdoll.h"
#include "Box2DHelper.h"

std::vector<Ragdoll*> ragdolls;

// Constructor de la clase Game
Game::Game(int ancho, int alto, std::string titulo)
{
	wnd = new RenderWindow(VideoMode(ancho, alto), titulo);// Inicializa la ventana de renderizado con el tamaño y título especificados
	wnd->setVisible(true);// Hace que la ventana sea visible
	fps = 60;// Establece el límite de frames por segundo
	wnd->setFramerateLimit(fps);
	frameTime = 1.0f / fps;// Calcula el tiempo de cada frame
	SetZoom();// Establece el zoom del juego
	InitPhysics();// Inicializa el mundo físico de Box2D
}

// Bucle principal del juego
void Game::Loop()
{
	// Bucle principal de juego, se ejecuta mientras la ventana esté abierta
	while (wnd->isOpen())
	{
		wnd->clear(clearColor);// Limpia la ventana con el color clearColor
		DoEvents();// Gestiona los eventos de la ventana
		CannonRotation(); //Actualizo el cañon
		UpdatePhysics();// Actualiza el mundo físico
		DrawGame();// Dibuja el juego
		wnd->display();// Muestra la ventana
	}
}

// Actualiza el mundo físico
void Game::UpdatePhysics()
{
	phyWorld->Step(frameTime, 8, 8);// Realiza un paso de simulación en el mundo físico
	phyWorld->ClearForces();// Limpia las fuerzas aplicadas en el mundo físico
	phyWorld->DebugDraw();// Dibuja el mundo físico en modo de depuración
}

void Game::DrawGame(){
	// Dibujar el cañon (cuerpo de control)
	sf::RectangleShape cannonShape(sf::Vector2f(15.0f, 10.0f));
	cannonShape.setFillColor(sf::Color::Green);
	cannonShape.setPosition(controlBody->GetPosition().x, controlBody->GetPosition().y);
	cannonShape.setOrigin(0, cannonShape.getSize().y / 2); // Origen en la base del cañón
	cannonShape.setRotation(controlBody->GetAngle() * 180 / b2_pi); // Box2D usa radianes, SFML grados
	wnd->draw(cannonShape);

	// Dibujar las paredes
	sf::RectangleShape leftWallShape(sf::Vector2f(10, alto)); // Alto de la ventana
	leftWallShape.setFillColor(sf::Color::Black);
	leftWallShape.setPosition(100, 0); // X = 100 para que comience donde termina el suelo
	wnd->draw(leftWallShape);

	// Dibujar el suelo
	sf::RectangleShape groundShape(sf::Vector2f(500, 5));
	groundShape.setFillColor(sf::Color::Red);
	groundShape.setPosition(0, 95);
	wnd->draw(groundShape);

	//Dibujar el techo
	sf::RectangleShape ceilinglShape(sf::Vector2f(500, 5));
	ceilinglShape.setFillColor(sf::Color::Red);
	ceilinglShape.setPosition(0, 0);
	wnd->draw(ceilinglShape);


}
// Gestiona los eventos de la ventana
void Game::DoEvents()
{
	// Obtener posición del mouse en coordenadas del mundo
	Vector2i mousePixel = Mouse::getPosition(*wnd);
	Vector2f mouseWorld = wnd->mapPixelToCoords(mousePixel);

	// Obtener posición del cañón en pixeles
	b2Vec2 cannonPosMeters = controlBody->GetPosition();
	Vector2f cannonPosPixels(cannonPosMeters.x, cannonPosMeters.y);

	// Calcular ángulo hacia el mouse
	float dx = mouseWorld.x - cannonPosPixels.x;
	float dy = mouseWorld.y - cannonPosPixels.y;
	float angle = std::atan2(dy, dx); //Radianes

	Event evt;
	while (wnd->pollEvent(evt))
	{
		switch (evt.type)
		{
		case Event::Closed:// Cierra la ventana si se presiona el botón de cerrar
			wnd->close();
			break;
		case Event::MouseButtonPressed:

			if (evt.mouseButton.button == Mouse::Left) {
				// Ángulo del cañón
				float angle = controlBody->GetAngle();
				b2Vec2 dir(std::cos(angle), std::sin(angle));

				// Posición de aparición del ragdoll (desde la punta del cañón)
				b2Vec2 spawnPos = controlBody->GetWorldCenter() + b2Vec2(dir.x * 15.0f, dir.y * 15.0f);

				// Calcular distancia al mouse para la fuerza
				Vector2i mousePixel = Mouse::getPosition(*wnd);
				Vector2f mouseWorld = wnd->mapPixelToCoords(mousePixel);
				b2Vec2 cannonPos(controlBody->GetPosition().x, controlBody->GetPosition().y);
				float dx = mouseWorld.x - cannonPos.x;
				float dy = mouseWorld.y - cannonPos.y;
				float distance = std::sqrt(dx * dx + dy * dy);
				float power = std::min(distance * 10.0f, 1000.0f); // Escala limitada

				// Crear el ragdoll
				Ragdoll* ragdoll = new Ragdoll(phyWorld, spawnPos, angle);
				b2Vec2 impulse(dir.x * power, dir.y * power);
				ragdoll->ApplyImpulse(impulse);
				ragdolls.push_back(ragdoll);
			}
			break;
		}
	}
}

// Definimos el area del mundo que veremos en nuestro juego
// Box2D tiene problemas para simular magnitudes muy grandes
// Configura el área visible del juego
void Game::SetZoom()
{
	// Define el área visible del juego
	View v(Vector2f(50.0f, 50.0f), Vector2f(100.0f, 100.0f));
	wnd->setView(v);
}

void Game::CannonRotation() {
	// Obtener posición del mouse en coordenadas del mundo
	Vector2i mousePixel = Mouse::getPosition(*wnd);
	Vector2f mouseWorld = wnd->mapPixelToCoords(mousePixel);

	// Obtener posición del cañón en pixeles
	b2Vec2 cannonPosMeters = controlBody->GetPosition();
	Vector2f cannonPosPixels(cannonPosMeters.x, cannonPosMeters.y); 

	// Calcular ángulo hacia el mouse
	float dx = mouseWorld.x - cannonPosPixels.x;
	float dy = mouseWorld.y - cannonPosPixels.y;
	float angle = std::atan2(dy, dx); //Radianes

	// Aplicar rotación
	controlBody->SetTransform(controlBody->GetPosition(), angle); 
}

void Game::InitPhysics()
{
	// Inicializamos el mundo con la gravedad por defecto
	phyWorld = new b2World(b2Vec2(0.0f, 9.8f));

	// Creamos el renderer de debug y le seteamos las banderas para que dibuje TODO
	debugRender = new SFMLRenderer(wnd);
	debugRender->SetFlags(UINT32_MAX);
	phyWorld->SetDebugDraw(debugRender);

	// Creamos un piso y paredes
	b2Body* groundBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
	groundBody->SetTransform(b2Vec2(50.0f, 100.0f), 0.0f);

	b2Body* leftWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
	leftWallBody->SetTransform(b2Vec2(0.0f, 50.0f), 0.0f);

	b2Body* rightWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
	rightWallBody->SetTransform(b2Vec2(100.0f, 50.0f), 0.0f);

	// Creamos un techo
	b2Body* topWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
	topWallBody->SetTransform(b2Vec2(50.0f, 0.0f), 0.0f);

	b2Body* pelota = Box2DHelper::CreateCircularDynamicBody(phyWorld, 2.0f, 1.0f, 1.0f, 0.3f);
	pelota->SetTransform(b2Vec2(60.0f, 20.0f), 0.0f);

	b2Body* plataforma = Box2DHelper::CreateRectangularStaticBody(phyWorld, 30.0f, 5.0f);
	plataforma->SetTransform(b2Vec2(60.0f, 22.0f), 0.0f);

	b2Body* box = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 6.0f, 6.0f, 0.5f, 0.3f, 0.3f);
	box->SetTransform(b2Vec2(80.0f, 94.0f), 0.0f);

	// Creación del cañón
	controlBody = Box2DHelper::CreateRectangularKinematicBody(phyWorld, 25, 10);
	controlBody->SetTransform(b2Vec2(5.0f, 100.0f), 0.0f);

}



Game::~Game(void)
{ }
