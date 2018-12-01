#pragma once

//おそらくmadwigkフィルタの有効化
#define AHRS


namespace hardware{

	class ShakingManager{

		public:

			ShakingManager(int _threshold_swing_angle_axis);

			void resetCount(){ count = 0; }	//カウント値リセット
			void stopCount(){ shaking_state = ShakingState::Stop; } 	//カウント再開/開始
			void startCount(){ shaking_state = ShakingState::CountingUpperSwing; }	//カウント一次停止
			int getCount(){ return count; };		//現在のカウント数

		protected:
			enum class ShakingState{
				CountingUpperSwing,
				CountingLowerSwing,
				Stop
			};
			const float threshold_swing_angle_axis;	//1カウントと見なす，角速度の大きさ(正の値)
			int counter;	//現在のカウント数
			const int SamplingPeriod;	
			ShakingState shaking_state;		//カウント計測の状態
			void updateMeasurement(); 	//IMU値の更新
			void updateCount();		//振った回数の更新


			// FreeRTOS によって実行される関数
			void task() {
				// handleEvent の定期実行
				portTickType xLastWakeTime = xTaskGetTickCount();
				while (1) {
					vTaskDelayUntil(&xLastWakeTime, PeriodMillis / portTICK_RATE_MS);

					updateMeasurement();
					updateCount();
				}
				vTaskDelete(NULL);
			}


	};


}
