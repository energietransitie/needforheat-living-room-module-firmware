#include <scd41_task.hpp>

#include <scd41.h>
#include <scheduler.hpp>
#include <secure_upload.hpp>
#include <measurements.hpp>
#include <util/error.hpp>

auto secureUploadQueue = SecureUpload::Queue::GetInstance();

void SCD41Task(void *taskInfo)
{
	auto taskName = Scheduler::GetName(taskInfo).c_str();

	// Add formatters for all the measurements.
	Measurements::Measurement::AddFormatter("co2__ppm", "%u");
	Measurements::Measurement::AddFormatter("temp_in__degC", "%.1f");
	Measurements::Measurement::AddFormatter("rel_humidity__0", "%.1f");

	co2_init(SCD41_ADDR);

	// Do three measurements. We do not save the first two.
	uint16_t scd41Data[3] = {};
	esp_err_t err;
	for (int i = 0; i < 3; i++)
	{
		err = co2_read(SCD41_ADDR, scd41Data);
		if (err != ESP_OK)
		{
			ESP_LOGW(taskName, "CRC was incorrect or no sensor wat attached.");
			return;
		}
	}

	Measurements::Measurement co2Concentration("co2__ppm", scd41Data[0]);
	secureUploadQueue.AddMeasurement(co2Concentration);

	Measurements::Measurement roomTemp("temp_in__degC", scd41_temp_raw_to_celsius(scd41Data[1]));
	secureUploadQueue.AddMeasurement(roomTemp);

	Measurements::Measurement relativeHumidity("rel_humidity__0", scd41_rh_raw_to_fraction(scd41Data[2]));
	secureUploadQueue.AddMeasurement(relativeHumidity);
}
