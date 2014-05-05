#!/usr/bin/python

import os
import sys
import cherrypy
import avmeta

current_dir = os.path.dirname( os.path.abspath( __file__ ) )

root = avmeta.AvMeta()

def application( environ, start_response ):
    cherrypy.config.update( {} )
    cherrypy.tree.mount( root, script_name="/" )
    return cherrypy.tree( environ, start_response )

if __name__ == '__main__':
    # cherrypy.quickstart( root, '/' )

    cherrypy.tree.mount(
        root, '/api/avmeta',
        {'/':
            {'request.dispatch': cherrypy.dispatch.MethodDispatcher()}
         }
    )

    cherrypy.engine.start()
    cherrypy.engine.block()
