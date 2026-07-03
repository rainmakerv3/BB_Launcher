// SPDX-FileCopyrightText: Copyright 2026 BBLauncher Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Bnd.h"
#include "ConflictHandler.h"
#include "Fmg.h"
#include "GameParam.h"
#include "Tpf.h"
#include "modules/ModMerger.h"

namespace fs = std::filesystem;

namespace FileHelper {

ConflictHandler::ConflictHandler(const std::string& fileType, ModMerger* merger)
    : BBFormat(merger) {
    type = fileType;
}

bool ConflictHandler::HandleItemConflict(std::vector<char>& origData, std::vector<char>& mod1Data,
                                         std::vector<char>& mod2Data, const std::string& filename) {
    if (type.contains("TPF") || type.contains("tpf")) {
        Tpf origTpf(origData, merger);
        if (!origTpf.HandleConflict(mod1Data, mod2Data)) {
            return false;
        } else {
            origData.clear();
            origTpf.RepackTpf(origData);
        }
    }

    if (type.contains("fmg")) {
        Fmg origFmg(origData, merger);
        if (!origFmg.HandleConflict(mod1Data, mod2Data)) {
            return false;
        } else {
            origData.clear();
            origFmg.RepackFmg(origData);
        }
    }

    if (type.contains("param")) {
        GameParam origPrm(origData, filename, merger);
        if (!origPrm.HandleConflict(mod1Data, mod2Data)) {
            return false;
        } else {
            origData.clear();
            origPrm.RepackGameParam(origData);
        }
    }

    return true;
}

bool ConflictHandler::HandleBinderConflict(std::vector<char>& origData, std::vector<char>& mod1Data,
                                           std::vector<char>& mod2Data) {
    Bnd origBnd = Bnd(origData, merger);
    const Bnd mod1Bnd = Bnd(mod1Data, merger);
    const std::vector<Bnd::BinderFile> mod1BndFiles = mod1Bnd.files;

    const Bnd mod2Bnd = Bnd(mod2Data, merger);
    const std::vector<Bnd::BinderFile> mod2BndFiles = mod2Bnd.files;

    for (auto& file : origBnd.files) {
        sendLog("Checking BND file: " + file.name);

        bool mod1Modified = false;
        bool mod2Modified = false;

        std::optional<Bnd::BinderFile> mod1file = origBnd.GetSameFile(file.id, mod1BndFiles);
        std::optional<Bnd::BinderFile> mod2file = origBnd.GetSameFile(file.id, mod2BndFiles);

        mod1Modified = mod1file && (mod1file->data != file.data);
        mod2Modified = mod2file && (mod2file->data != file.data);

        if (mod1Modified && mod2Modified) {
            std::string ext = std::filesystem::path(file.name).extension().string();
            bool extractable = ext == ".fmg" || ext == ".tpf" || ext == ".param";

            if (extractable) {
                ConflictHandler fileHandler = ConflictHandler(ext, merger);
                if (!fileHandler.HandleItemConflict(file.data, mod1file->data, mod2file->data,
                                                    file.name)) {
                    return false;
                } else {
                    file.compressedSize = file.data.size();
                    file.uncompressedSize = file.data.size();
                }
            } else {
                if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                    QMetaObject::invokeMethod(merger, &ModMerger::OpenPriorityDialog,
                                              Qt::BlockingQueuedConnection);
                    if (merger->GetModPriority() == ModMerger::ModPriority::NotSet) {
                        return false;
                    }
                }

                if (merger->GetModPriority() == ModMerger::ModPriority::Mod1) {
                    file = mod1file.value();
                    sendLog("Unresolvable conflict, using binder file " + mod1file.value().name +
                                " from prioritized mod: " + merger->Mod1Name(),
                            LogFormat::Yellow);
                } else if (merger->GetModPriority() == ModMerger::ModPriority::Mod2) {
                    file = mod2file.value();
                    sendLog("Unresolvable conflict, using binder file " + mod2file.value().name +
                                " from prioritized mod: " + merger->Mod2Name(),
                            LogFormat::Yellow);
                }
            }
        }

        if (mod1Modified && !mod2Modified) {
            file = mod1file.value();
            sendLog("Merging modified binder file " + mod1file.value().name +
                    " from mod: " + merger->Mod1Name());
        } else if (mod2Modified && !mod1Modified) {
            file = mod2file.value();
            sendLog("Merging modified binder file " + mod2file.value().name +
                    " from mod: " + merger->Mod2Name());
        }

        sendLog("File processed: " + file.name + "\n");
    }

    std::unordered_set<int> existingIds;
    for (const auto& file : origBnd.files) {
        existingIds.insert(file.id);
    }

    for (const auto& file : mod1BndFiles) {
        if (existingIds.find(file.id) == existingIds.end()) {
            origBnd.files.push_back(file);
            sendLog("Bnd file added: " + file.name);
        }
    }

    for (const auto& file : mod2BndFiles) {
        if (existingIds.find(file.id) == existingIds.end()) {
            origBnd.files.push_back(file);
            sendLog("Bnd file added: " + file.name);
        }
    }

    origData.clear();
    origBnd.RepackBnd(origData);

    return true;
}

ConflictHandler::~ConflictHandler() {}

} // namespace FileHelper
