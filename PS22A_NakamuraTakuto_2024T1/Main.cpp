﻿# include <Siv3D.hpp>

/*
	古き良き書き方での実装
	・安全性や利便性などは一切考えていない
*/

namespace constracts {

	namespace ball{
		/// @brief ボールの速さ
		constexpr double SPEED = 480.0;
	};

	namespace brick {
		/// @brief ブロックのサイズ
		constexpr Size SIZE{ 40, 20 };

		/// @brief ブロックの数　縦
		constexpr int Y_COUNT = 5;

		/// @brief ブロックの数　横
		constexpr int X_COUNT = 20;

		/// @brief 合計ブロック数
		constexpr int MAX = Y_COUNT * X_COUNT;
	};
};

class Ball {
public:
	/// @brief ボールの速度
	Vec2 velocity;

	//@brief ボール
	Circle ball;

	Ball() : velocity(0, -constracts::ball::SPEED), ball(400, 400, 8) {}

	void Draw() {
		// ボール描画
    	ball.draw();
	}
};

class Bricks {
public:
	/// @brief ブロック
	Rect bricks[constracts::brick::MAX];

	Bricks(){
		using namespace constracts::brick;

		// ブロックを初期化
		for (int y = 0; y < Y_COUNT; ++y) {
			for (int x = 0; x < X_COUNT; ++x) {
				int index = y * X_COUNT + x;
				bricks[index] = Rect{
					x * SIZE.x,
					60 + y * SIZE.y,
					SIZE
				};
			}
		}
	}

	void Intersecte(Ball* target) {
		using namespace constracts::brick;

		 ///ブロックとの衝突を検知
			for (int i = 0; i < MAX; ++i) {
				// 参照で保持
				Rect& refBrick = bricks[i];

			// 衝突を検知
			if (refBrick.intersects(target))
			{
				// ブロックの上辺、または底辺と交差
				if (refBrick.bottom().intersects(target) || refBrick.top().intersects(target))
				{
					target->velocity.y *= -1;
				}
				else // ブロックの左辺または右辺と交差
				{
					target->velocity.x *= -1;
				}

				// あたったブロックは画面外に出す
				refBrick.y -= 600;

				// 同一フレームでは複数のブロック衝突を検知しない
				break;
			}
		}
	 }
};

class Padddle {
public:
	// パドル
	const Rect paddle;

	Padddle() : paddle { Arg::center(Cursor::Pos().x, 500), 60, 10 } {}
};

void Main()
{
	using namespace constracts::ball;
	 

//#pragma endregion
//
//	while (System::Update())
//	{
//		//==============================
//		// 更新
//		//==============================
//		// パドル
//		const Rect paddle{ Arg::center(Cursor::Pos().x, 500), 60, 10 };
//
//		// ボール移動
//		ball.moveBy(ballVelocity * Scene::DeltaTime());
//
//		//==============================
//		// コリジョン
//		//==============================
//		// ブロックとの衝突を検知
//		for (int i = 0; i < MAX; ++i) {
//			// 参照で保持
//			Rect& refBrick = bricks[i];
//
//			// 衝突を検知
//			if (refBrick.intersects(ball))
//			{
//				// ブロックの上辺、または底辺と交差
//				if (refBrick.bottom().intersects(ball) || refBrick.top().intersects(ball))
//				{
//					ballVelocity.y *= -1;
//				}
//				else // ブロックの左辺または右辺と交差
//				{
//					ballVelocity.x *= -1;
//				}
//
//				// あたったブロックは画面外に出す
//				refBrick.y -= 600;
//
//				// 同一フレームでは複数のブロック衝突を検知しない
//				break;
//			}
//		}
//
//		// 天井との衝突を検知
//		if ((ball.y < 0) && (ballVelocity.y < 0))
//		{
//			ballVelocity.y *= -1;
//		}
//
//		// 壁との衝突を検知
//		if (((ball.x < 0) && (ballVelocity.x < 0))
//			|| ((Scene::Width() < ball.x) && (0 < ballVelocity.x)))
//		{
//			ballVelocity.x *= -1;
//		}
//
//		// パドルとの衝突を検知
//		if ((0 < ballVelocity.y) && paddle.intersects(ball))
//		{
//			ballVelocity = Vec2{
//				(ball.x - paddle.center().x) * 10,
//				-ballVelocity.y
//			}.setLength(BALL_SPEED);
//		}
//
//		//==============================
//		// 描画
//		//==============================
//		// ブロック描画
//		for (int i = 0; i < MAX; ++i) {
//			bricks[i].stretched(-1).draw(HSV{ bricks[i].y - 40 });
//		}
//
//		// ボール描画
//		ball.draw();
//
//		// パドル描画
//		paddle.rounded(3).draw();
//	}
}
