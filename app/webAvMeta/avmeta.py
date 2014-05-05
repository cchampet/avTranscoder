# -*- coding: utf-8 -*-

import os
import json
import cherrypy
from cherrypy import _cperror

from AvTranscoder import AvTranscoder

class AvMeta(object):
    exposed = True

    @cherrypy.tools.json_out()
    def GET(self, id=None):
        if not id:
            raise cherrypy.HTTPError( "404 Bad request", "Invalid request parameters." )

        #filename = getFilenameFromId( id )
        filename = "/mnt/backup/PADdef/PRestore_PADdef_20131205_WrapMXF_VF_AD_VM_SP_1-6_7-8_9-10_15-16.mxf"

        inputFile = AvTranscoder.InputFile( filename )
        inputFile.analyse()

        properties = inputFile.getProperties()
        print properties.filename
        print properties.duration

        result = {
            'id': id,
            'filename': properties.filename,
            'formatName': properties.formatName,
            'formatLongName': properties.formatLongName,
            'streamsCount': properties.streamsCount,
            'programsCount': properties.programsCount,
            'startTime': properties.startTime,
            'duration': properties.duration,
            'bitRate': properties.bitRate,
            'packetSize': properties.packetSize
        }

        videoProperties = []
        audioProperties = []

        for videoStream in properties.videoStreams:
            videoProperties.append( {
                'codecName': videoStream.codecName,
                'codecLongName' : videoStream.codecLongName,
                'profileName' : videoStream.profileName,
                'colorTransfert' : videoStream.colorTransfert,
                'colorspace' : videoStream.colorspace,
                'colorRange' : videoStream.colorRange,
                'colorPrimaries' : videoStream.colorPrimaries,
                'chromaSampleLocation' : videoStream.chromaSampleLocation,
                'fieldOrder' : videoStream.fieldOrder,
                'pixelName' : videoStream.pixelName,
                'endianess' : videoStream.endianess,
                'startTimecode' : videoStream.startTimecode,
                'timeBase' : {
                    'num': videoStream.timeBase.num,
                    'den': videoStream.timeBase.den
                },
                'sar' : {
                    'num': videoStream.sar.num,
                    'den': videoStream.sar.den
                },
                'dar' : {
                    'num': videoStream.dar.num,
                    'den': videoStream.dar.den
                },
                'streamId' : videoStream.streamId,
                'codecId' : videoStream.codecId,
                'bitRate' : videoStream.bitRate,
                'maxBitRate' : videoStream.maxBitRate,
                'minBitRate' : videoStream.minBitRate,
                'ticksPerFrame' : videoStream.ticksPerFrame,
                'width' : videoStream.width,
                'height' : videoStream.height,
                'gopSize' : videoStream.gopSize,
                'dtgActiveFormat' : videoStream.dtgActiveFormat,
                'referencesFrames' : videoStream.referencesFrames,
                'profile' : videoStream.profile,
                'level' : videoStream.level,
                'componentsCount' : videoStream.componentsCount,
                'chromaWidth' : videoStream.chromaWidth,
                'chromaHeight' : videoStream.chromaHeight,
                'fps' : videoStream.fps,
                'hasBFrames' : videoStream.hasBFrames,
                'indexedColors' : videoStream.indexedColors,
                'bitWisePacked' : videoStream.bitWisePacked,
                'hardwareAcceleration' : videoStream.hardwareAcceleration,
                'notFirstPlane' : videoStream.notFirstPlane,
                'rgbPixelData' : videoStream.rgbPixelData,
                'pseudoPaletted' : videoStream.pseudoPaletted,
                'asAlpha' : videoStream.asAlpha,
                'isInterlaced' : videoStream.isInterlaced,
                'topFieldFirst' : videoStream.topFieldFirst
            } )

        result[ 'video' ] = videoProperties

        for audioStream in properties.audioStreams:
            audioProperties.append( {
                'codecName': audioStream.codecName,
                'codecLongName': audioStream.codecLongName,
                'sampleFormat': audioStream.sampleFormat,
                'streamId': audioStream.streamId,
                'codecId': audioStream.codecId,
                'sampleRate': audioStream.sampleRate,
                'channels': audioStream.channels,
                'bit_rate': audioStream.bit_rate
            } )

        result[ 'audio' ] = audioProperties

        return result
