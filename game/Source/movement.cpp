/* Copyright Steve Rabin, 2010. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright Steve Rabin, 2010"
 */

#include "DXUT.h"
#include "database.h"
#include "movement.h"
#include "gameobject.h"
//#include "body.h"
#include "tiny.h"
#include "statemch.h"
#include "world.h"
#include "attacker.h"


extern World					g_World;

Movement::Movement( GameObject& owner )
: m_owner( &owner ),
  m_speedWalk( 1.f / 5.7f ),
  m_speedJog( 1.f / 2.3f ),
  m_facing(0),
  m_size(.5f),
  m_movementMode(MOVEMENT_NULL)
{
	m_position.x = m_position.y = m_position.z = m_target.x = m_target.y = m_target.z = 0.0f;

    WCHAR str[MAX_PATH];
    // hack fix later
    ( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, L"level.grd" ) );
    // Load the world file
    g_terrain.Load(str);
}

Movement::~Movement( void )
{

}

void Movement::Animate( double dTimeDelta )
{

    if( m_movementMode == MOVEMENT_COMPUTE_A_STAR )
    {
        ComputePathAStar();
    }

    if( m_movementMode == MOVEMENT_SEEK_TARGET )
    {
        // determine facing
        D3DXVECTOR3 vfacing;
        D3DXVec3Subtract(&vfacing, &m_target, &m_position);
        float fDist = D3DXVec3Length( &vfacing );
        D3DXVec3Normalize( &vfacing, &vfacing);

        if( fDist < 0.01f )
        {
            // send a message that the NPC has reached the target
            g_database.SendMsgFromSystem( m_owner, MSG_Arrived );
        }

        else
        {
            // figure out how far to move this frame
            D3DXVECTOR3 deltaPos = (m_diffPosition) *.4f * (float)dTimeDelta; // mulitply by the speed of the npc

            m_position += deltaPos;

            // there might be a bug here...
            if( vfacing.z == 0 )
            {
                if( vfacing.x < 0 )
                    m_facing = -D3DX_PI/2;
                else
                    m_facing = D3DX_PI/2;
            }
            else if( vfacing.z > 0.f )
                m_facing = -acosf( vfacing.x) + D3DX_PI/2;
            else
                m_facing = acosf( vfacing.x ) + D3DX_PI/2;


            // change the facing direction of the NPC
            m_owner->getNPC()->m_facing = m_facing;
            // change the position of the NPC
            m_owner->getNPC()->SetPosition( m_position );
            m_owner->getNPC()->setFacingVec( vfacing);
        }
    }
    else if( m_movementMode == MOVEMENT_WAYPOINT_LIST && m_waypointList.size() > 0 )  // use the waypoint list
    {
        D3DXVECTOR3 target = m_waypointList.front();
        D3DXVECTOR3 toTarget = target - m_position;

        D3DXVECTOR3 goal = m_waypointList.back();
        D3DXVECTOR3 toGoal = goal - m_position;

        if( D3DXVec3Length( &toGoal ) < 0.01f )
        {	//Notify goal reached
            m_waypointList.clear();
            m_movementMode = MOVEMENT_NULL;
            g_database.SendMsgFromSystem( m_owner, MSG_Arrived );
        }
        else
        {	
            if( D3DXVec3Length( &toTarget ) < 0.01f )
            {	//Target reached - get new target
                m_waypointList.pop_front();
                target = m_waypointList.front();
                toTarget = target - m_position;
            }

            D3DXVec3Normalize( &toTarget, &toTarget);
        
            // figure out how far to move this frame
            D3DXVECTOR3 deltaPos = (toTarget) *.4f * (float)dTimeDelta; // mulitply by the speed of the npc
        
            float temp;
           
            // there might be a bug here...
            if( toTarget.z <= 0.001 && toTarget.z>= -0.001 )
            {
                if( toTarget.x < 0 )
                    temp = -D3DX_PI/2;
                else
                    temp = D3DX_PI/2;
            }
            else if( toTarget.z  > 0.f )
                temp = -acosf( toTarget.x ) + D3DX_PI/2;
            else
                temp = acosf( toTarget.x ) + D3DX_PI/2;


            /*if( abs(temp - m_facing) > .5f)
            {
                float diff = (m_facing - temp) *.2f * (float)dTimeDelta;
                m_facing +=diff;
            }
            m_owner->getNPC()->m_facing =  m_facing; */
            /*if( abs(temp - m_facing) > .35f)
            {
                // rotate
               // if( temp - m_facing < 0)
                {
                    m_facing += (temp) *2.2f * (float)dTimeDelta;
                    m_owner->getNPC()->m_facing =  m_facing; 
                }
                /*else
                {
                    m_facing -= (temp) *2.2f * (float)dTimeDelta;
                    m_owner->getNPC()->m_facing =  m_facing;
                }
               /* if( m_facing > D3DX_PI )
                    m_facing = D3DX_PI;
                else if( m_facing < -D3DX_PI)
                    m_facing -= D3DX_PI;
                    
            }
            else
                m_owner->getNPC()->m_facing = m_facing = temp;
              */  
            if( m_facing != temp )
                m_owner->getNPC()->m_facing = m_facing = temp;


            // do we need to rotate first
            /*if( (D3DXVec3Dot( &toTarget, &m_vec_facing )) < .9f)
            {
                m_facing += (temp) *.4f * (float)dTimeDelta;
                m_owner->getNPC()->m_facing =  m_facing;
                m_vec_facing += (toTarget) *.4f * (float)dTimeDelta;
                m_vec_facing.y = 0;
                D3DXVec3Normalize(&m_vec_facing, &m_vec_facing);
                //m_owner->getNPC()->setFacingVec( (toTarget) *.4f * (float)dTimeDelta);    
            }
           /* else if( m_facing - temp < 1.5f)
            {
                m_facing -= (temp) *.4f * (float)dTimeDelta;
                m_owner->getNPC()->m_facing = m_facing;
            }
            */
            
            //else
            {
                // update the postition
                m_position += deltaPos;
                //m_owner->getNPC()->SetPosition( m_position );
                m_owner->getNPC()->setFacingVec( toTarget);
            }
            // set the postition 
            m_owner->getNPC()->SetPosition( m_position );

        }
    }
    else if(m_movementMode == MOVEMENT_SCALE)
    {
        if( m_size > .01f )
        {
            m_size -= .1f * (float)dTimeDelta;
            m_owner->getNPC()->setScale(m_size);
            // rotate the NPC as it dies
            m_owner->getNPC()->m_facing += (g_time.GetCurTime()*.003f);
            
        }
        else
        {
            m_movementMode = MOVEMENT_NULL;
        }
    }
}


void Movement::SetIdleSpeed( void )
{
//	m_owner->GetBody().SetSpeed( 0.0f );
	//m_owner->GetTiny().SetIdleKey( true );
}

void Movement::SetWalkSpeed( void )
{
//	m_owner->GetBody().SetSpeed( m_speedWalk );
	//m_owner->GetTiny().SetMoveKey();
}

void Movement::SetJogSpeed( void )
{
//	m_owner->GetBody().SetSpeed( m_speedJog );
	//m_owner->GetTiny().SetMoveKey();
}

void Movement::setTarget( D3DXVECTOR3 &target)
{
    m_target = target;
    m_diffPosition = m_target - m_position;
    // normalize the diff position so the NPC will always move the same speed
    D3DXVec3Normalize(&m_diffPosition,&m_diffPosition);
    
    // clear the lists
    OpenList.clear();
    ClosedList.clear();
    m_waypointList.clear();
    m_movementMode = MOVEMENT_SEEK_TARGET;
    /*
    //if( m_waypointList.empty())
        m_waypointList.push_back( target );
    
*/
}

void Movement::setRandomTarget()
{
    D3DXVECTOR3 goToPosition;
    bool wall = true;
    float val = 1.f;
    int i = 0;
    while( (wall || val > 0) && (wall || i < 6)) // limit the number of times through the loop
    {
        // set to random location between 0 and 25
        goToPosition.x = 2 + 21.0f * rand() / RAND_MAX;
        goToPosition.z = 2 + 21.0f * rand() / RAND_MAX; // need to add 1 to the z position because of the map coords
        goToPosition.y = 0;

        wall = g_terrain.IsWall((int)goToPosition.z, (int)goToPosition.x);
        val = g_World.getGridLayer()->getValue((int)goToPosition.z, (int)goToPosition.x);
        ++i;
    }
    // update the target
    setDestLocation( goToPosition );
}


// set the A* destination
void Movement::setDestLocation( D3DXVECTOR3 pos )
{
    m_target = pos;

    g_terrain.GetRowColumn(&pos, &m_row, &m_col);

    m_movementMode = MOVEMENT_COMPUTE_A_STAR;

    // empty the lists
    OpenList.clear();
    ClosedList.clear();
    m_waypointList.clear();
    // clear the debug list
    g_terrain.resetDebugList();

}

// helper for sorting the open and closed lists
bool node_cmp( gameNode first, gameNode second )
{
    if( first.total < second.total )
        return true;
    else
        return false;
}

// compute the A* path
void Movement::ComputePathAStar()
{
    float xDiff;
    float yDiff;
    bool finished = false;
    int i = 0;

    if(OpenList.size() == 0)
    {
        // figrue out the start node
        int curR, curC;
        D3DXVECTOR3 cur = m_position;
        g_terrain.GetRowColumn( &cur, &curR, &curC );

        // create a node
        gameNode n;
        n.Col = curC;
        n.Row = curR;
        n.parentCol = -1;
        n.parentRow = -1;
       // n.parentPtr = NULL;
        n.cost = 0;  // cost is 0 since we are at the start
        xDiff = pow((float)(n.Row - m_row), 2);
        yDiff = pow((float)(n.Col - m_col), 2);
        n.total = sqrt( xDiff + yDiff ) + n.cost;

        // push start node onto the open list
        OpenList.push_back(n);
        // debug
        g_terrain.setOpenList( n.Row, n.Col );
        
    }

    // if open list is empty, then no path possible
    // loop until we are out of items
    while( OpenList.size() > 0 )
    {
        // sort the open list
        OpenList.sort( node_cmp );

        // pop the best node off open list - B
        gameNode b = OpenList.front();
        
        // if B is the goal node then path found
        if( b.Row == m_row && b.Col == m_col )
        {
            finished = true;
            // add this to the closed list for ease
            ClosedList.push_back(b);
            break; // we are at the end
        }

        OpenList.pop_front();

        // find all neighboring nodes 
        for( int newR = b.Row - 1; newR < b.Row + 2; ++newR)
        {
            for( int newC = b.Col - 1; newC < b.Col + 2; ++newC)
            {
                bool wall = g_terrain.IsWall( newR, newC ); // find out if this node is a wall
                // make sure this node make sense greater than 0, less than the map size and not equal to the current node
                if( !wall && newR >=0 && newC >= 0 && (newR != b.Row || newC != b.Col) && newC < 25 /*width*/ && newR < 25 /*height*/ )
                {
                    // check for neighboring walls
                    if( (g_terrain.IsWall(newR + (b.Row - newR), newC) || g_terrain.IsWall( newR, newC + (b.Col - newC))))
                        continue;
                    // for each neighboring node - c
                    gameNode c;
                    c.Col = newC;
                    c.Row = newR;
                    c.parentCol = b.Col;
                    c.parentRow = b.Row;
                    //c.parentPtr = &b;

                    // compute the cost 
                    xDiff = pow((float)(c.Row - b.Row), 2);
                    yDiff = pow((float)(c.Col - b.Col), 2);
                    c.cost = b.cost + sqrt( xDiff + yDiff); // the distance from c to its parent
                    
                    // heuristic cost, distance to the goal
                   /* if( this->m_heuristicCalc == 0 )
                    {
                        // euclidian heuristic
                        xDiff = pow((float)(c.Row - m_row), 2);
                        yDiff = pow((float)(c.Col - m_col), 2);

                        c.total = sqrt( xDiff + yDiff )*this->m_heuristicWeight + c.cost;
                    }
                    else
                    */
                    {
                       // xDiff = (float)(c.Row - b.Row);
                       // yDiff = (float)(c.Col - b.Col);
                       // c.cost = b.cost + min(xDiff,yDiff) * sqrt(2.f) + max(xDiff,yDiff) - min(xDiff,yDiff); // the distance from c to its parent
                        // cardinal/intercardna heuristic
                        xDiff = (float)abs(c.Row - m_row);
                        yDiff = (float)abs(c.Col - m_col);
                        c.total = (min(xDiff,yDiff) * sqrt(2.f) + max(xDiff,yDiff) - min(xDiff,yDiff))* (1.01f) /*m_heuristicWeight*/ + c.cost - g_World.getGridLayer()->getVisValue(c.Row,c.Col);
                    }

                    // is C on the closed list?
                    std::list<gameNode>::const_iterator itrc;
                    bool foundc = false;
                    for( itrc = ClosedList.begin(); itrc != ClosedList.end(); ++itrc )
                    {
                        if( ((gameNode)*itrc).Row == c.Row && ((gameNode)*itrc).Col == c.Col)
                        {
                            foundc = true;
                            break;
                        }
                    }

                    // if C is not on the open list
                    bool found = false;
                    std::list<gameNode>::const_iterator itr;
                    for( itr = OpenList.begin(); itr != OpenList.end(); ++itr )
                    {
                        if( ((gameNode)*itr).Row == c.Row && ((gameNode)*itr).Col == c.Col)
                        {
                            found = true;
                            break;
                        }
                    }

                   /* if( found == false && !foundc) // not on the open list or the closed list
                    {
                        // add to open list
                        OpenList.push_back(c);
                        g_terrain.SetColor( c.Row, c.Col, DEBUG_COLOR_BLUE );
                    } else if( (foundc && c.total < ((Node)*itrc).total) || (found && c.total < ((Node)*itr).total))
                    {
                        // if c is on the closed with with a lower cost
                        if( foundc ) // remove from closed list
                            ClosedList.erase( itrc );
                            // remove from closed list and open
                        // update open list, not sure if this works
                        if( found )
                            OpenList.erase(itr);

                        OpenList.push_back( c );
                        g_terrain.SetColor( c.Row, c.Col, DEBUG_COLOR_BLUE );
                    }*/
                    if( found == false && !foundc || (foundc == true && c.total < ((gameNode)*itrc).total)|| (found && c.total < ((gameNode)*itr).total))
                    {
                        // if c is on the closed with with a lower cost
                        if( foundc ) // remove from closed list
                            ClosedList.erase( itrc );
                            // remove from closed list and open
                        // update open list, not sure if this works
                        if( found )
                            OpenList.erase(itr);

                        OpenList.push_back( c );
                        // debug
                        g_terrain.setOpenList( c.Row, c.Col );
                    }
                }
            }
        }
        
        // add B to closed list
        ClosedList.push_back(b);
        ++i;
        // debug
        g_terrain.setClosedList( b.Row, b.Col );
            // check time, or single step true
        if( i > 10 ) // this could be tweaked
            break;
            
                // abort and resume next frame
            
        
    }// finished 

    // load the waypoints up
    if( ClosedList.size() > 0 && finished == true)
    {
        m_movementMode = MOVEMENT_WAYPOINT_LIST;
    
        // always make sure the waypoint list is cleared
        m_waypointList.clear();

        // compute the final path using rubberbanding
        ComputeRubberbandPath();

        // smooth out the final path using splines
        SmoothCurrentWaypoints();
        
    }
    else if( OpenList.size() == 0)
    {
        // TODO - HANDLE THIS CASE!
        // seek a random location since we can't get there anyway
    }
}



void Movement::ComputeRubberbandPath()
{
    // iterate through the closed list
    bool foundc = false;
    gameNode last = ClosedList.back();
    ClosedList.pop_back();
    
    // add the last coordinate (will always need to be added)
    D3DXVECTOR3 spot = g_terrain.GetCoordinates( last.Row, last.Col );
    m_waypointList.push_front( spot );

    gameNode n1;
    gameNode n2;
    if(ClosedList.size() > 0)
    {
        // find gameNode n1, middle gameNode
        do
        {
            n1 = ClosedList.back();
            ClosedList.pop_back();
        } while( (n1.Col != last.parentCol || n1.Row != last.parentRow) && ClosedList.size() > 0);
    }

    while( ClosedList.size() > 0)
    {
        // find n2
        do
        {
            n2 = ClosedList.back();
            ClosedList.pop_back();
        }while( (n2.Col != n1.parentCol || n2.Row != n1.parentRow) && ClosedList.size() > 0 );

        // check to see if n1 can be dumped 
        bool wall_found = false;

        // loop through the area defined by the outer gameNodes, n2 and last
        for( int i = min(n2.Col, last.Col ); i < max(n2.Col, last.Col ) + 1 && !wall_found; ++i)
        {
            for( int j = min(n2.Row, last.Row ); j < max(n2.Row, last.Row )+1; ++j)
            {
                if( g_terrain.IsWall(j,i) )
                {
                    wall_found = true;
                    break;
                }
            }
        }
        
        // found a wall?
        if( wall_found == true )
        {
            // add the middle to the waypoint list
            spot = g_terrain.GetCoordinates( n1.Row, n1.Col );
            m_waypointList.push_front( spot ); 
            // set last to the middle
            last = n1;
            // set the middle to the outer
            n1 = n2;
        }
        else
        {
            // drop the middle gameNode
            n1 = n2;
        }
    }
}




// smootht the final path using splines
void Movement::SmoothCurrentWaypoints( void )
{
    //float u[3] = {.25f, .5f, .75f};
    float u[9] = { .1f, .2f, .3f, .4f,.5f,.6f,.7f,.8f,.9f};
    D3DXVECTOR3 point0;
    D3DXVECTOR3 point1;
    D3DXVECTOR3 point2;
    D3DXVECTOR3 point3;
    D3DXVECTOR3 new_point;

    std::list<D3DXVECTOR3>::iterator itr;
    std::list<D3DXVECTOR3>::iterator insert_itr;

    std::list<D3DXVECTOR3> temp_list;

    int size = (int) m_waypointList.size();
    int i = 0;

    point0 = m_position;
    // loop through the waypoint list and add waypoints if neccessary
   
  //  if(m_rubberband)   
    {
        for( itr = m_waypointList.begin(); itr != m_waypointList.end(), i < size; ++itr, ++i)
        {
            point1 = *itr;

            D3DXVECTOR3 diff = (point1) - (point0);
            // if the distance between the points is greater than 1.2
            if( D3DXVec3Length(&diff) > 1.4f)
            {
                D3DXVECTOR3 dir (1.f, 0, 1.f );
                D3DXVec3Normalize(&point3,&(point1 -point0));
                // add a point
                point3.x *= dir.x;
                point3.z *= dir.z;
                point3  += point0;
                /*diff = point3 - point0;
                if( D3DXVec3Length(&diff) > .05f)

                point3 = (point1 - point0)/2 + point0;
                */
                m_waypointList.insert(itr, point3);
                --itr;
                --i;
                point0 = point3;
            }
            else
                point0 = point1;
        }
    }


    size = (int) m_waypointList.size();
    // set the initial 0 position
    point0 = *m_waypointList.begin();
    for(itr = m_waypointList.begin(), i =0; itr != m_waypointList.end() && i < size -1; ++itr, ++i)
    {
        insert_itr = itr;
        
        // set point 1
        point1 = *insert_itr;
        ++insert_itr;

        // set point 2
        point2 = *insert_itr;
        //insert_itr = (itr)+2;
        if( insert_itr != m_waypointList.end() && i < size - 2)
            ++insert_itr;
       
        // set point 3
        point3 = *insert_itr;

        // run create spine points
        for( int i = 0; i < 9; ++i)
        {
            D3DXVec3CatmullRom(&new_point, &point0, &point1, &point2, &point3, u[i]);
            temp_list.push_back(new_point);

        }
        
        point0 = *itr;
    }

    // add the temp points to the waypoint list
    itr = m_waypointList.begin();
    ++itr;
    for( ; itr != m_waypointList.end(); ++itr)
    {
        for( int i = 0; i < 9 && temp_list.size() > 0; ++i)
        {
            m_waypointList.insert(itr, temp_list.front());
            temp_list.pop_front();
        }
    }

// remove any duplicates
    point0 = m_waypointList.front();
    //D3DXVec3Normalize(&point0,&point0);
    itr = m_waypointList.begin();
    ++itr;
    size = m_waypointList.size();
    for(int i =0;  itr != m_waypointList.end() && i < size -2; ++i )
    {
        //if( itr != m_waypointList.end() )
        {
            point1 = *itr;
            point2 = point1 - point0;
            D3DXVec3Normalize(&point2,&point2);
            ++itr;
            point3 = *itr - point0;
            D3DXVec3Normalize(&point3,&point3);
            if( m_waypointList.size() > 1 )
                --itr;
            else
                itr = m_waypointList.begin();
            float dot =  D3DXVec3Dot( &point1, &point0);
            // check if the vectors are facing the same direction
            if( D3DXVec3Dot( &point3, &point2) > .99999993f )
            {
                insert_itr = itr;
                ++itr;
                // erase the waypoint
                m_waypointList.erase(insert_itr);
                //size = m_waypointList.size();
                //--i;
            }
            else
            {
                ++itr;
                point0 = point1;
            }
        }
    }
    
}


void Movement::DrawDebugVisualization( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj )
{
	//Draw waypoints
	if( !m_waypointList.empty() )
	{
		D3DXVECTOR3 p0, p1;
		D3DXVECTOR3 last = *m_waypointList.begin();
		D3DXVECTOR3 cur = m_position;
		p0 = last;
		last.y = cur.y = p0.y = 0.5f;
		
        //g_debugdrawing.DrawLine( pd3dDevice, pViewProj, cur, p0, DEBUG_COLOR_RED, true );
        m_owner->getDebugLine()->setPosition(cur, p0);
        m_owner->getDebugLine()->Render(pd3dDevice, *pViewProj);

		WaypointList::iterator i = m_waypointList.begin();
		for( WaypointList::iterator i = m_waypointList.begin(); i != m_waypointList.end(); i++ )
		{
			p0 = *i;
			p0.y = 0.5f;
			//p1 = p0;
			//p1.y += 0.03f;
			//g_debugdrawing.DrawLine( pd3dDevice, pViewProj, p0, p1, DEBUG_COLOR_BLACK, false );
			//g_debugdrawing.DrawLine( pd3dDevice, pViewProj, last, p0, DEBUG_COLOR_RED, true );
            m_owner->getDebugLine()->setPositionArray(last, p0);
           
            // render a vertical line
            p1 = p0;
            p1.y = .0f;
            m_owner->getDebugLine()->setPositionArray(p0, p1);
            
            last = p0;
		}
        m_owner->getDebugLine()->RenderArray( pd3dDevice, *pViewProj );
	}

    // draw seeking debug line
    if( m_movementMode == MOVEMENT_SEEK_TARGET )
    {
        // always have the debug line point from the target to the destination
        D3DXVECTOR3 p1,p2;
        p1 = m_target;
        p1.y = .5f;
        p2 = m_position;
        p2.y = .5f;
        m_owner->getDebugLine()->setPosition(p1, p2);
        m_owner->getDebugLine()->Render(pd3dDevice, *pViewProj);
    }
}

void Movement::DrawAstarDebug( IDirect3DDevice9* pd3dDevice, D3DXMATRIX* pViewProj )
{
    // save the color 
    D3DXCOLOR orig = m_owner->getDebugLine()->getColor();
    D3DXVECTOR3 point,point1;
    // Draw closed list
    for(int i =0; i < 25; ++i)
    {
        for( int j = 0; j < 25; ++j)
        {
            if(g_terrain.getList( i, j) != WorldFile::NOT_USED )
            {
                // set the color
                (g_terrain.getList( i, j) == WorldFile::OPEN_LIST) ? m_owner->getDebugLine()->setColor( D3DCOLOR_COLORVALUE(0,0,1,0) ) : 
                    m_owner->getDebugLine()->setColor( D3DCOLOR_COLORVALUE(0,1,0,0) );

                // render a square using lines
                m_owner->getDebugLine()->setPositionSquare(i, j);
            }
        }
    }
    m_owner->getDebugLine()->RenderArray(pd3dDevice, *pViewProj );
    // reset the color
    m_owner->getDebugLine()->setColor( orig );
}

void Movement::setMovementDead() 
{ 
    m_movementMode = MOVEMENT_SCALE; 
}

void Movement::reset()
{
    m_position = m_orig_position; 
    m_size = .5f; 
    m_owner->getNPC()->setScale(m_size);
    m_owner->SetHealth(2);
    // clear the waypoint list
    m_waypointList.clear();

    m_movementMode = MOVEMENT_NULL;
    m_owner->getNPC()->SetPosition( m_position ); 
    m_facing = 0;
    m_owner->getNPC()->m_facing = m_facing;
    m_owner->getNPC()->setFacingVec(D3DXVECTOR3(0,0,1.f));
}

// sets the initail position
void Movement::setStartPosition( D3DXVECTOR3& position )
{ 
    m_position = m_orig_position = position; 
    m_owner->getNPC()->SetPosition( m_position ); 
}