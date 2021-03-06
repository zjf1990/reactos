/*
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS text-mode setup
 * FILE:            base/setup/usetup/filesup.h
 * PURPOSE:         File support functions
 * PROGRAMMER:
 */

#pragma once

NTSTATUS
SetupCreateDirectory(
    PWCHAR DirectoryName);

NTSTATUS
SetupCopyFile(
    PWCHAR SourceFileName,
    PWCHAR DestinationFileName);

NTSTATUS
SetupExtractFile(
    PWCHAR CabinetFileName,
    PWCHAR SourceFileName,
    PWCHAR DestinationFileName);


BOOLEAN
IsValidPath(
    IN PCWSTR InstallDir);

/* EOF */
