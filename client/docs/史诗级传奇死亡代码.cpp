    if(m_localPlayer) {
    // handle world state update 
    bool hasstateupdate = false;
    uint32_t last_ticks = 0, rlast_ticks = 0;
    unsigned char wasd[4]; 
    unsigned char space;
    {
        std::lock_guard<std::mutex> lock(m_stateMutex);
        if(!i_isStateConsumed) { // if not consumed, consume now
            i_isStateConsumed = true; // mark as consumed
            // spdlog::info("world get update state, size: {}", m_EntitiesBufferFront->size());
            std::unique_ptr<Entity> hero_ind = nullptr;
            for(auto& state: *m_EntitiesBufferFront) {
                cout << state.get() << endl;
                if(state->GetID() == m_localPlayer) {
                    hero_ind = std::move(state);
                }
            }
            if(hero_ind) { 
                Player* rhero = dynamic_cast<Player*>(hero_ind.get());
                LocalPlayer* lhero = dynamic_cast<LocalPlayer*>(m_entities[m_localPlayer].get());
                uint32_t oldest_seq = lhero->GetOldestSeq();
                wasd[0] = lhero->m_wasd[0];
                wasd[1] = lhero->m_wasd[1];
                wasd[2] = lhero->m_wasd[2];
                wasd[3] = lhero->m_wasd[3];
                space = lhero->m_space;
                if(rhero && oldest_seq <= rhero->GetLatestSeq()) { // has value to update

                    // ---------------------------

                    uint32_t new_oldest_seq = rhero->GetLatestSeq();
                    rlast_ticks = rhero->m_lastTicks; // use later 

                    auto& afterqueue = lhero->m_inputQueue; 
                    while(
                        !afterqueue.empty() && 
                        afterqueue.front().first.sequence_number <= new_oldest_seq
                        ) {
                            afterqueue.pop_front(); // 删掉早于该次更新的输入
                        }
                   last_ticks = lhero->m_lastTicks;
                    auto newqueue = std::move(afterqueue);

                    m_entities.clear(); // clear all entities
                    for(auto& ent: *m_EntitiesBufferFront) {
                        // TODO: 增量式更新 
                        if(!ent) continue; 
                        uint32_t id = ent->GetID();
                        spdlog::debug("id = {}", id);
                        if(id == 0) continue; 
                        if(id != m_localPlayer) {
                            m_entities[id] = std::move(ent); // move entity to world
                        } 
                    }

                    auto newlocalhero = std::make_unique<LocalPlayer>(*rhero);
                    newlocalhero->m_inputQueue = std::move(newqueue);
                    m_entities[m_localPlayer] = std::move(newlocalhero);
                    auto& xxent = m_entities[m_localPlayer];
                    LocalPlayer* xxhero = dynamic_cast<LocalPlayer*>(xxent.get());
                    xxhero->m_wasd[0] = wasd[0];
                    xxhero->m_wasd[1] = wasd[1];
                    xxhero->m_wasd[2] = wasd[2];
                    xxhero->m_wasd[3] = wasd[3];
                    xxhero->m_space = space;
                    hasstateupdate = true;
                    // ---------------------------

                }
                    
            }


        }
    }

    if(hasstateupdate) {
        auto her= dynamic_cast<LocalPlayer*>(GetEntity(m_localPlayer));
        auto que = her->m_inputQueue;
        if(!que.empty()) {
            uint32_t oldest_ticks = que.front().second; 
            if(oldest_ticks < rlast_ticks) { // 如果服务端已模拟的ticks多于下一次输入前的ticks
                spdlog::warn("server has simulated more ticks than next input. client : {}, server: {}", oldest_ticks, rlast_ticks);
                oldest_ticks = 0; 
            } else {
                oldest_ticks -= rlast_ticks;
            }
            for(int i = 0; i < oldest_ticks; i ++) {
                // WorldPhysicsUpdate();
                // WorldAnimeUpdate(); 
            }
            // her->PushNewInput(que.front().first);
            que.pop_front(); 
        }
        
        while(!que.empty()) {
            uint32_t ticks = que.front().second;
            for(int i = 0; i < ticks; i ++) {
                // WorldPhysicsUpdate();
                // WorldAnimeUpdate(); 
            }
            // her->PushNewInput(que.front().first);
            que.pop_front();
        }
        for(int i = 0; i < last_ticks; i ++) {
            // WorldPhysicsUpdate();
            // WorldAnimeUpdate(); 
        }
    }
    }

    else {

        std::lock_guard<std::mutex> lock(m_stateMutex);
        if(!i_isStateConsumed) { // if not consumed, consume now
            m_entities.clear(); // clear all entities
            for(auto& ent: *m_EntitiesBufferFront) {
                // TODO: 增量式更新 
                if(!ent) continue; 
                uint32_t id = ent->GetID();
                spdlog::debug("id = {}", id);
                if(id == 0) continue; 
            }
            i_isStateConsumed = true;
        }

    }