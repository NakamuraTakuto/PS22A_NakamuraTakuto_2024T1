# include <Siv3D.hpp>

/*
	よりC++ライクな書き方
	・クラスベース
	・継承を行う
*/

//==============================
// 前方宣言
//==============================
class Ball;
class Bricks;
class Paddle;

//==============================
// 定数
//==============================
namespace constants {
	namespace brick {
		/// @brief ブロックのサイズ
		constexpr Size SIZE{ 40, 20 };

		/// @brief ブロックの数　縦
		constexpr int Y_COUNT = 5;

		/// @brief ブロックの数　横
		constexpr int X_COUNT = 20;

		/// @brief 合計ブロック数
		constexpr int MAX = Y_COUNT * X_COUNT;
	}

	namespace ball {
		/// @brief ボールの速さ
		constexpr double SPEED = 480.0;
	}

	namespace paddle {
		/// @brief パドルのサイズ
		constexpr Size SIZE{ 60, 10 };
	}

	namespace reflect {
		/// @brief 縦方向ベクトル
		constexpr Vec2 VERTICAL{ 1, -1 };

		/// @brief 横方向ベクトル
		constexpr Vec2 HORIZONTAL{ -1,  1 };
	}

	namespace system {
		 bool Is_GameStart = false;
		 bool Is_GameOver = false;
		 int life = 3;
	}
}

//==============================
// クラス宣言
//==============================
/// @brief ボール
class Ball final {
private:
	/// @brief 速度
	Vec2 velocity;

	/// @brief ボール
	Circle ball;

public:
	/// @brief コンストラクタ
	Ball() : velocity({ 0, -constants::ball::SPEED }), ball({ 400, 400, 8 }) {}

	/// @brief デストラクタ
	~Ball() {}

	/// @brief 更新
	void Update() {
		ball.moveBy(velocity * Scene::DeltaTime());
	}

	/// @brief 描画
	void Draw() const {
		ball.draw();
	}

	Circle PosReset()  {
		ball.setPos(400, 400);
		return ball;
	}

	Circle GetCircle() const {
		return ball;
	}

	Vec2 GetVelocity() const {
		return velocity;
	}

	/// @brief 新しい移動速度を設定
	/// @param newVelocity 新しい移動速度
	void SetVelocity(Vec2 newVelocity) {
		using namespace constants::ball;
		velocity = newVelocity.setLength(SPEED);
	}

	/// @brief 反射
	/// @param reflectVec 反射ベクトル方向 
	void Reflect(const Vec2 reflectVec) {
		velocity *= reflectVec;
	}
};

/// @brief ブロック
class Bricks final {
private:
	/// @brief ブロックリスト
	Rect brickTable[constants::brick::MAX];

public:
	/// @brief コンストラクタ
	Bricks() {
		using namespace constants::brick;

		for (int y = 0; y < Y_COUNT; ++y) {
			for (int x = 0; x < X_COUNT; ++x) {
				int index = y * X_COUNT + x;
				brickTable[index] = Rect{
					x * SIZE.x,
					60 + y * SIZE.y,
					SIZE
				};
			}
		}
	}

	/// @brief デストラクタ
	~Bricks() {}

	/// @brief 衝突検知
	void Intersects(Ball* const target);

	/// @brief 描画
	void Draw() const {
		using namespace constants::brick;

		for (int i = 0; i < MAX; ++i) {
			brickTable[i].stretched(-1).draw(HSV{ brickTable[i].y - 40 });
		}
	}
};

/// @brief パドル
class Paddle final {
private:
	Rect paddle;

public:
	/// @brief コンストラクタ
	Paddle() : paddle(Rect(Arg::center(Cursor::Pos().x, 500), constants::paddle::SIZE)) {}

	/// @brief デストラクタ
	~Paddle() {}

	/// @brief 衝突検知
	void Intersects(Ball* const target) const;

	/// @brief 更新
	void Update() {
		paddle.x = Cursor::Pos().x - (constants::paddle::SIZE.x / 2);
	}

	/// @brief 描画
	void Draw() const {
		paddle.rounded(3).draw();
	}
};

/// @brief 壁
class Wall {
public:
	/// @brief 衝突検知
	static void Intersects(Ball* target) {
		using namespace constants;

		if (!target) {
			return;
		}

		auto velocity = target->GetVelocity();
		auto ball = target->GetCircle();

		// 天井との衝突を検知
		if ((ball.y < 0) && (velocity.y < 0))
		{
			target->Reflect(reflect::VERTICAL);
		}

		// 壁との衝突を検知
		if (((ball.x < 0) && (velocity.x < 0))
			|| ((Scene::Width() < ball.x) && (0 < velocity.x)))
		{
			target->Reflect(reflect::HORIZONTAL);
		}
	}
};

//@gamemanager クリア、ゲームオーバー時の処理
class GameManager {
private:
	//残機の数
	int life = constants::system::life;
	//表示する残機（円）を管理する配列
	Circle* lifeArr;

public:
	~GameManager() {
		delete[] lifeArr;
	}

	///  描画する残機を生成し、配列に格納
	void LifeSetter() {
		lifeArr = new Circle[life]; //メモ、実際に同じことをやる場合はVectorの方が良い

		for (int i = 0; i < life; i++) {
			lifeArr[i] = Circle{ 20 + (30 * i), 15, 8 };
		}
	}

	///落下判定
	void Intersects(Ball* target);

	void Draw()  {
		for (int i = 0; i < life; i++) {
			lifeArr[i].draw();
		}
	}
};

//==============================
// 定義
//==============================
void Bricks::Intersects(Ball* const target) {
	using namespace constants;
	using namespace constants::brick;

	if (!target) {
		return;
	}

	auto ball = target->GetCircle();

	for (int i = 0; i < MAX; ++i) {
		// 参照で保持
		Rect& refBrick = brickTable[i];

		// 衝突を検知
		if (refBrick.intersects(ball))
		{
			// ブロックの上辺、または底辺と交差
			if (refBrick.bottom().intersects(ball)
				|| refBrick.top().intersects(ball))
			{
				target->Reflect(reflect::VERTICAL);
			}
			else // ブロックの左辺または右辺と交差
			{
				target->Reflect(reflect::HORIZONTAL);
			}

			// あたったブロックは画面外に出す
			refBrick.y -= 600;

			// 同一フレームでは複数のブロック衝突を検知しない
			break;
		}
	}
}

void Paddle::Intersects(Ball* const target) const {
	if (!target) {
		return;
	}

	auto velocity = target->GetVelocity();
	auto ball = target->GetCircle();

	if ((0 < velocity.y) && paddle.intersects(ball))
	{
		target->SetVelocity(Vec2{
			(ball.x - paddle.center().x) * 10,
			-velocity.y
		});
	}
}

void GameManager::Intersects(Ball* target) {
	using namespace constants;

	if (!target) {
		return;
	}

	Circle ball = target->GetCircle();

	//ボールの落下判定
	if (ball.y >= 600)
	{
		//落下した時にライフを1減らす
		life -= 1;

		if (life <= 0) {//残り残機がないとき
			//残機表示を減らす
			lifeArr[life].x = -600;
			//ゲーム処理を一部停止
			system::Is_GameStart = false;
			system::Is_GameOver = true;
		}
		else {//残機がまだあるとき
			//ボールの座標移動処理
			target->PosReset();
			////Velocityのリセット
			target->SetVelocity({ 0, -ball::SPEED });
			//ゲーム処理を一部停止
			system::Is_GameStart = false;
			//残機表示を減らす
			lifeArr[life].x = -600;
		}
	}
}

//==============================
// エントリー
//==============================
void Main()
{
	using namespace constants::system;

	Bricks bricks;
	Ball ball;
	Paddle paddle;
	GameManager gamemanager;

	//残機要のCircleを生成。配列に格納
	gamemanager.LifeSetter();

	//テキストの前準備
	Font font{ FontMethod::MSDF, 48 };

	while (System::Update())
	{
		//=============================
		// ゲーム開始のToF切り替え
		// ============================
		if (KeyEnter.down()) {
			Is_GameStart = true;
		}
		//==============================
		// 更新
		//==============================
		if (Is_GameStart) {
			paddle.Update();
			ball.Update();
		}

		//==============================
		// コリジョン
		//==============================
		bricks.Intersects(&ball);
		Wall::Intersects(&ball);
		paddle.Intersects(&ball);
		gamemanager.Intersects(&ball);

		//==============================
		// GameOver時のテキスト描画
		// =============================
		if (Is_GameOver) {
			font(U"GameOver").drawAt(40, 400, 300, ColorF{ 255, 0, 0 });
		}
		//==============================
		// 描画
		//==============================
		bricks.Draw();
		ball.Draw();
		paddle.Draw();
		gamemanager.Draw();
	}
}
