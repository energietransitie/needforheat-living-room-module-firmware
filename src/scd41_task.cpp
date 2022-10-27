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
	Measurements::Measurement::AddFormatter("CO2concentration", "%u");
	Measurements::Measurement::AddFormatter("roomTemp", "%.1f");
	Measurements::Measurement::AddFormatter("relativeHumidity", "%.1f");

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

	Measurements::Measurement co2Concentration("CO2concentration", scd41Data[0]);
	secureUploadQueue.AddMeasurement(co2Concentration);

	Measurements::Measurement roomTemp("roomTemp", scd41_temp_raw_to_celsius(scd41Data[1]));
	secureUploadQueue.AddMeasurement(roomTemp);

	Measurements::Measurement relativeHumidity("relativeHumidity", scd41_rh_raw_to_percent(scd41Data[2]));
	secureUploadQueue.AddMeasurement(relativeHumidity);
}
