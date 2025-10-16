//
// Created by thoma on 03/04/2025.
//




#include "include/AeolusSynthesizer.h"
#include "../../SynthesizerBase/include/OboeAudioPlayer.h"
#include "../UserInterface/android_aeolus_user_interface.h"
#include "../MidiInterface/MidiAndoidAeolus.h"


namespace Aeolussynthesizer {

    AeolusSynthesizer::AeolusSynthesizer( Lfq_u32 *qnote, Lfq_u32 *qcomm,Lfq_u8* qmidi,
                                         const char *stopsPath)
                                         : AeolusAudio("AeolusAudio", qnote, qcomm),
                                           _ui{std::make_unique<android_aeolus_user_interface>()},
                                           _qmidi(qmidi),
                                           stop_directory("stops/stops"),
                                           instrument_directory("stops/Aeolus"),
                                           wave_directory("waves"),
                                           _midiInterface{std::make_unique<MidiAndroidAeolus>(qnote, qmidi, midimap(), "Aeolus Midi")}
                                           {

         setStopsPath(stopsPath);


        _defaultOscillator = std::make_unique<Aeolussynthesizer::AeolusOscillator>(this);
        _fsamp=synthesizerBase::samplingRate; // Defined from the synthesizer base clase
        _audioPlayer =
                std::make_unique<synthesizerBase::OboeAudioPlayer>(_defaultOscillator.get(),
                                                                   synthesizerBase::samplingRate);
        _nplay=synthesizerBase::OboeAudioPlayer::defaultChannels;
        if(_nplay>2)
        {
            _nplay=2;
        }
        _fsize=synthesizerBase::OboeAudioPlayer::defaultFrameSize;

        // Normally, the audio buffer size should not change anymore, we should be able to allocate
        // it already at this point of time
        allocateOutputBuffer();



        // midimap should be audio->midimap but this needs to be reviewed...
        // This needs to be preserved , otherwise the c string becomes invalid
        static std::string const s_stop = std::string(
                std::string(std::string(_stopsPath) + "/") + stop_directory);
        static std::string const s_inst = std::string(
                std::string(std::string(_stopsPath) + "/") + instrument_directory);
        static std::string const s_wave = std::string(
                std::string(std::string(_stopsPath) + "/") + wave_directory);


        const char *full_stop_directory = s_stop.c_str();
        const char *full_instrument_directory = s_inst.c_str();
        const char *full_wave_directory = s_wave.c_str();

        model = new Model(qcomm, _qmidi, _midimap, "aeolus",
                          full_stop_directory,
                          full_instrument_directory, full_wave_directory, false);
        slave = new Slave();
        ITC_ctrl::connect(this, EV_EXIT, &itcc, EV_EXIT);
        ITC_ctrl::connect (this, EV_QMIDI, model, EV_QMIDI);
        ITC_ctrl::connect(this, TO_MODEL, model, FM_AUDIO);
        ITC_ctrl::connect (_midiInterface.get(), EV_EXIT,  &itcc, EV_EXIT);
        ITC_ctrl::connect (_midiInterface.get(), EV_QMIDI, model, EV_QMIDI);
        ITC_ctrl::connect (_midiInterface.get(), TO_MODEL, model, FM_IMIDI);
        ITC_ctrl::connect(model, EV_EXIT, &itcc, EV_EXIT);
        ITC_ctrl::connect(model, TO_AUDIO, this, FM_MODEL);
        ITC_ctrl::connect(model, TO_SLAVE, slave, FM_MODEL);
        ITC_ctrl::connect(model, TO_IFACE, _ui.get(), FM_MODEL);
        ITC_ctrl::connect(slave, EV_EXIT, &itcc, EV_EXIT);
        ITC_ctrl::connect(slave, TO_AUDIO, this, FM_SLAVE);
        ITC_ctrl::connect(slave, TO_MODEL, model, FM_SLAVE);
        ITC_ctrl::connect(_ui.get(), EV_EXIT, &itcc, EV_EXIT);
        ITC_ctrl::connect(_ui.get(), TO_MODEL, model, FM_IFACE);
        if (model->thr_start(SCHED_FIFO, relpri() - 30, 0)) {

            model->thr_start(SCHED_OTHER, 0, 0);

        }
        slave->thr_start(SCHED_OTHER, 0, 0);
        _ui->thr_start(SCHED_OTHER, 0, 0);
        _midiInterface->open_midi (); // no thread is really required since this will be called
        AeolusSynthesizer::start(); // After the midi level, transmit audio information
        // via the jni, so just open the interface to signal that things are going the way the should


    }





    //_defaultOscillator->onAudioConnected();}

    AeolusSynthesizer::~AeolusSynthesizer(){
        _audioPlayer = nullptr;
        _defaultOscillator = nullptr;
        for (int i = 0; i < _nplay; i++) delete[] _outbuf [i];
        delete[] _stopsPath;
    }


   void  AeolusSynthesizer::play() {
        if(!isPlaying) {
            std::lock_guard<std::mutex> lock(_mutex);
            _audioPlayer->play();
            isPlaying=true;
        }

    }

   void  AeolusSynthesizer::stop() {
        if(isPlaying) {
            std::lock_guard<std::mutex> lock(_mutex);
            _audioPlayer->stop();
            isPlaying=false;
        }
    }



    /**
     * This gets the actual audio source configured in the audio player
     * Bewasre: If it is not of type JUCEOscillator, but of the base type AudioSource or of some other type
     * this function will return null.
     * @return pointer to the audio source currently in use, null if not of type JUCEOscillator (or subtype)
     */
    AeolusOscillator* AeolusSynthesizer::getAudioSource(){
        return(dynamic_cast<AeolusOscillator *>(Synthesizer::getAudioSource()));
    }

    void AeolusSynthesizer::useDefaultOscillator(){
        _audioPlayer->setAudioSource(_defaultOscillator.get());

    }

    void AeolusSynthesizer::setVolume(float volume)
    {

    }


    void AeolusSynthesizer::setStopsPath(const char* stopsPath)
    {
        _stopsPath = new char[strlen(stopsPath) + 1];
        strcpy(const_cast<char*>(_stopsPath), stopsPath);


    }



    void AeolusSynthesizer::start() {
        init_audio();
        if (thr_start(SCHED_FIFO, relpri() - 30, 0)) {

            thr_start(SCHED_OTHER, 0, 0);

        }

        AeolusAudio::start();


    }

    void AeolusSynthesizer::fillAudioBuffer(float *audioData, int32_t framesCount,
                                            oboe::ChannelCount channelCount) {


        if((framesCount ==0) | (channelCount ==0)) return;


        unsigned long start=ITC_ctrl::delay ()/1000;
        if((_fsize!=framesCount) | (_nplay != channelCount))
        {

            if((_fsize*_nplay) != 0)
            {
                // This is not good, output buffer is allocated but has the wrong size
                // deallocate previously allocated buffer
                for (int i = 0; i < _nplay; i++) delete[] _outbuf [i];

            }
            // We need to allocate new memory for holding the audio output

            _nplay=channelCount;
            _fsize=framesCount;
            allocateOutputBuffer();
        }



        proc_queue (_qnote);
        proc_queue (_qcomm);
        proc_keys1 ();
        proc_keys2 ();

        proc_synth(_fsize);


        for(int i=0; i<_nplay; i++) {
            for (int j = 0; j < _fsize; j++) {

                audioData[j] = _outbuf[i][j];
            }
        }



        //proc_mesg();
        unsigned long stop=ITC_ctrl::delay ()/1000;
        unsigned long delta_t=stop-start;
        if(delta_t>5) {
            __android_log_print(android_LogPriority::ANDROID_LOG_INFO,
                                "AeolusSynthesizer",
                                "fill Audio Buffer long execution time: %d ms",(int)-delta_t);
        }

    }

    void AeolusSynthesizer::allocateOutputBuffer() {
        if((_fsize>0) & (_nplay >0)) {
            for (int i = 0; i < _nplay; i++) {
                _outbuf[i] = new float[_fsize];

            }

        }
    }

    void AeolusSynthesizer::noteon(int chan, int key, int vel) {
        Imidi::MidiEvent E{};
        E.type=SND_SEQ_EVENT_NOTEON;
        E.note.note=key;
        E.note.channel=chan;
        E.note.velocity=vel;


        _midiInterface->proc_midi_event(E);
    }

    void AeolusSynthesizer::noteoff(int chan, int key, int vel) {
        Imidi::MidiEvent E{};
        E.type=SND_SEQ_EVENT_NOTEOFF;
        E.note.note=key;
        E.note.channel=chan;
        E.note.velocity=vel;




        _midiInterface->proc_midi_event(E);

    }

    void AeolusSynthesizer::activateRank(int division_id, int rank_id) {
        int bit_mask=255;
        if( division_id >= _ndivis)
        {
            __android_log_print(android_LogPriority::ANDROID_LOG_WARN,
                                "AeolusSynthesizer", "Division %d does not exist",division_id);
            return;
        }

        if (_qnote->write_avail () > 0)
        {


           _qnote->write (0, (7 << 24) | ((division_id & 255) << 16) | ((rank_id & 255) << 8) | (bit_mask & 255));

           _qnote->write_commit (1);



        }


    }

    void AeolusSynthesizer::stopRank(int division_id, int rank_id) {
        int bit_mask=0;
        if( division_id >= _ndivis)
        {
            __android_log_print(android_LogPriority::ANDROID_LOG_WARN,
                                "AeolusSynthesizer", "Division %d does not exist",division_id);
            return;
        }

        if (_qnote->write_avail () > 0)
        {


            _qnote->write (0, (7 << 24) | ((division_id & 255) << 16) | ((rank_id & 255) << 8) | (bit_mask & 255));

            _qnote->write_commit (1);



        }
    }

    void AeolusSynthesizer::thr_main() {
        _running=true;
        while(_running)
        {
            proc_mesg ();
        }
    }

    bool AeolusSynthesizer::isInitializing() {
        if (_ui == nullptr)
        {
            __android_log_print(android_LogPriority::ANDROID_LOG_WARN,
                                "AeolusSynthesizer::isInitializing", "UI not initialized yet");
            return true;
        }
        return _ui->isInitializing();
    }

    int AeolusSynthesizer::get_n_divisions() {
        if (_ui == nullptr)
        {
            return 0;
        }
        return _ui->get_n_divisions();
    }


    const char* AeolusSynthesizer::getLabelForDivision(int division_index){
        if(_ui == nullptr)
        {
            return "";
        }
        return _ui->getLabelForDivision(division_index);
    }

    int AeolusSynthesizer::get_n_stops_for_division(int division_index) {
        Group* division_group = model->getGroupWithLabel(getLabelForDivision(division_index));
        if(division_group==nullptr)
        {
            return 0;
        }
        int n_stops=0;
        for(int i=0; i<division_group->_nifelm; i++)
        {
            if(division_group->_ifelms[i]._type==Ifelm::DIVRANK) n_stops++;
        }
        return n_stops;
    }



    const char *AeolusSynthesizer::getLabelForStop(int index_division, int index_stop) {
        Ifelm* theStop= getIfelmForStop(index_division, index_stop);
        if(theStop==nullptr)
        {
            return "";
        }
        return theStop->_label;

    }

    int AeolusSynthesizer::getIfelmIndexForStop(int index_division, int index_stop) {
        return getAbsoluteInterfaceElementIndex(index_division,index_stop,Ifelm::DIVRANK);
    }

    void AeolusSynthesizer::activateStop(int division_id, int stop_id) {
        int theStopIndex = getIfelmIndexForStop(division_id, stop_id);


        if(theStopIndex<0)
        {
            return;
        }


        send_event (TO_MODEL, new M_ifc_ifelm ( MT_IFC_ELSET, division_id, theStopIndex));



    }

    bool AeolusSynthesizer::getStopActivated(int index_division, int index_stop) {
        Ifelm *theStop = getIfelmForStop(index_division,index_stop);
        if(theStop==nullptr)
        {
            return false;
        }
        if(theStop->_state > 0) return true;

        return false;
    }

    int AeolusSynthesizer::maxRanksPerStop() {
        return max_rank_in_stops;
    }



    Ifelm *AeolusSynthesizer::getIfelmForStop(int index_division, int index_stop) {
        // Get the absolute index in the interface element group list
        int ifelm_index = getIfelmIndexForStop(index_division,
                index_stop);
        // The interface element could not be found, return nullptr
        if(ifelm_index < 0) { return nullptr;}

        // Get the appropriate group for the division
        Group* division_group = model->getGroupWithLabel(getLabelForDivision(index_division));

        // If we can't find the group, return null pointer
        if(division_group==nullptr)
        {
                return nullptr;
        }



         return &(division_group->_ifelms[ifelm_index]);




    }

    int AeolusSynthesizer::getAbsoluteInterfaceElementIndex(int index_division,
                                                              int index_stop_within_type,
                                                              Ifelm::Ifelm_type element_type) {
        Group* division_group = model->getGroupWithLabel(getLabelForDivision(index_division));
        int return_index=-1;

        if(division_group==nullptr)
        {
            return -1;
        }
        int n_stops=0;
        for(int i=0; i<division_group->_nifelm; i++)
        {
            if(division_group->_ifelms[i]._type==element_type){
                if(n_stops==index_stop_within_type)
                {
                    return_index=i;
                }
                n_stops++;
            }
        }

        return return_index;
    }

    void AeolusSynthesizer::deactivateStop(int division_id, int stop_id) {
        int theStopIndex = getIfelmIndexForStop(division_id, stop_id);


        if(theStopIndex<0)
        {
            return;
        }


        send_event (TO_MODEL, new M_ifc_ifelm ( MT_IFC_ELCLR, division_id, theStopIndex));



    }



    bool AeolusSynthesizer::division_has_tremulant(int division_index) {
        int tremulant_index=getIfelmIndexForTremulant(division_index);
        if(tremulant_index>=0) return true;
        return false;
    }

    int AeolusSynthesizer::getIfelmIndexForTremulant(int index_division) {
        return getAbsoluteInterfaceElementIndex(index_division,
                0,Ifelm::TREMUL);
    }


    void AeolusSynthesizer::activateTremulantForDivision(int division_id) {
        int theTremulantIndex = getIfelmIndexForTremulant(division_id);


        if(theTremulantIndex<0)
        {
            return;
        }


        send_event (TO_MODEL, new M_ifc_ifelm ( MT_IFC_ELSET, division_id, theTremulantIndex));



    }

    void AeolusSynthesizer::deactivateTremulantForDivision(int division_id) {
        int theTremulantIndex = getIfelmIndexForTremulant(division_id);


        if(theTremulantIndex<0)
        {
            return;
        }


        send_event (TO_MODEL, new M_ifc_ifelm ( MT_IFC_ELCLR, division_id, theTremulantIndex));



    }

    int AeolusSynthesizer::get_n_tunings() {
        return model->get_n_tunings();
    }

    const char *AeolusSynthesizer::getTuningLabel(int index_tuning) {
        return model->getTuningLabel(index_tuning);
    }

    int AeolusSynthesizer::getCurrentTuning() {
        return model->getCurrentTuning();
    }

    float AeolusSynthesizer::getBaseFrequency() {
        return model->getBaseFrequency();
    }

    void AeolusSynthesizer::retune(int temperament, float base_frequency) {

        send_event (TO_MODEL, new M_ifc_retune(base_frequency,temperament));
    }

    bool AeolusSynthesizer::is_retuning() {
        return model->is_retuning();
    }


}


// AeolusSynthesizer