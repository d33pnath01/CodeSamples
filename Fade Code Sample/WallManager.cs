using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WallManager : MonoBehaviour
{
    #region MemberVariables
    // Tunnel Prefabs
    [SerializeField]
    private GameObject[] earlyTunnelPrefabs = null;
    [SerializeField]
    private GameObject[] midTunnelPrefabs = null;
    [SerializeField]
    private GameObject[] lateTunnelPrefabs = null;

    // Location to Generate
    [SerializeField]
    private Transform playerTransform = null;
    private float tunnelSpawnLocationZ = 0.0f;

    // Tunnel Details
    [SerializeField]
    private float tunnelLength = 60.0f;
    //private int tunnelsOnScreen = 2;

    // Keep Track of Active Tunnels
    private List<GameObject> activeTunnels = null;
    private int lastTunnelIndex = 0;
    private int numberOfTunnelsGenerated = 0;
    #endregion

    #region Start
    void Start()
    {
        // Holds the tunnel objects on the screen
        activeTunnels = new List<GameObject>();
        SpawnTunnel(earlyTunnelPrefabs);
    }
    #endregion

    #region Update
    void Update()
    {
        // Spawning Tunnels
        if(playerTransform.position.z > (tunnelSpawnLocationZ - (tunnelLength / 2)))
        {
            if(numberOfTunnelsGenerated < 3)
            {
                SpawnTunnel(midTunnelPrefabs);
            }
            else
            {
                SpawnTunnel(lateTunnelPrefabs);
            }
        }

        // Deleting Tunnels
        if(activeTunnels.Count > 2)
        {
            DeleteTunnel();
        }
    }
    #endregion

    #region SpawnTunnel
    private void SpawnTunnel(GameObject[] tunnelPrefabs)
    {
        GameObject tunnel;
        tunnel = Instantiate(tunnelPrefabs[RandomTunnelIndex(tunnelPrefabs)]) as GameObject;
        tunnel.transform.position = Vector3.forward * tunnelSpawnLocationZ;
        tunnelSpawnLocationZ += tunnelLength;
        numberOfTunnelsGenerated++;
        activeTunnels.Add(tunnel);
        tunnel.transform.SetParent(this.transform);
    }
    #endregion

    #region DeleteTunnel
    private void DeleteTunnel()
    {
        Destroy(activeTunnels[0]);
        activeTunnels.RemoveAt(0);
    }
    #endregion

    #region RandomTunnelIndex
    // Random Index Generator
    private int RandomTunnelIndex(GameObject[] tunnelPrefabs)
    {
        if(tunnelPrefabs.Length <= 1)
        {
            return 0;
        }

        int randomIndex = lastTunnelIndex;
        while(randomIndex == lastTunnelIndex)
        {
            randomIndex = Random.Range(0, tunnelPrefabs.Length);
        }

        lastTunnelIndex = randomIndex;
        return randomIndex;
    }
    #endregion
}
