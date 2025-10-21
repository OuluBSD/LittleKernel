
template <class Sample>
void SoundThread<Sample>::RecordCallback(StreamCallbackArgs& args) {
	double samplerate = snd.GetSampleRate();
	bool was_recording = is_recording;
	CheckEnd(args);
	
	if (meter.IsNull())
		meter.Create();
	auto& meter = *this->meter.data;
	int meter_sample_count = max<int>(1, (int)(samplerate * meter_duration));
	meter.data.SetCount(meter_sample_count, 0);
	meter_index = meter_index % meter_sample_count;
	Sample* meter_begin = meter.data.Begin();
	Sample* meter_end = meter.data.End();
	Sample* meter_it = meter_begin + meter_index;
	const Sample *rptr = (const Sample*)args.input;
	if (is_recording) {
		if (!was_recording) {
			if (current.data)
				current.data->updating = false;
			WhenClipEnd(current);
			if (phrase && mgr)
				mgr->OnPhraseEnd(*phrase);
			current.Create();
			current.data->channels = 1;
			current.data->samplerate = samplerate;
			current.data->updating = true;
			WhenClipBegin(current);
			if (msg) {
				phrase = &msg->Add(); // calls OnPhraseBegin
				phrase->BeginClip(current);
			}
		}
		auto& cdata = *current.data;
		int index = cdata.data.GetCount();
		cdata.data.Reserve(index + args.fpb);
		if (args.input != NULL) {
			for (uint32 i=0; i<args.fpb; i++) {
				auto v = *rptr++;
				cdata.data.Add(v);
				*meter_it++ = v;
				if (meter_it == meter_end)
					meter_it = meter_begin ;
			}
		}
	}
	else {
		if (args.input != NULL) {
			for (uint32 i=0; i<args.fpb; i++) {
				*meter_it++ = *rptr++;
				if (meter_it == meter_end)
					meter_it = meter_begin ;
			}
		}
	}
	meter_index = (meter_index + args.fpb) % meter_sample_count;
	
	if(!running) {
		args.state=SND_COMPLETE;
		stopped = true;
	}
}

template<class Sample>
double SoundThread<Sample>::GetPeakValue() const {
	if (meter.IsEmpty())
		return 0;
	
	if (!meter.IsEmpty()) {
		const Sample* it = meter.Begin();
		const Sample* it_end = meter.End();
		double peak = 0;
		while (it != it_end) {
			Sample val = *it++;
			double dbl = fabs(SampleToDouble<Sample>(val));
			peak = max(peak, dbl);
		}
		return peak;
	}
	return 0;
}

template<class Sample>
double SoundThread<Sample>::GetVolume() const {
	if (meter.IsEmpty())
		return 0;
	
	if (!meter.IsEmpty()) {
		int count = meter.GetCount();
		double sum = 0;
		const Sample* it = meter.Begin();
		const Sample* it_end = meter.End();
		while (it != it_end) {
			Sample val = *it++;
			double dbl = fabs(SampleToDouble<Sample>(val));
			sum += dbl;
		}
		double av = sum / (double)count;
		return av;
	}
	return 0;
}

