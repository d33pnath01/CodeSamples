using System.Collections;
using UnityEngine;

public class RotateTunnel : MonoBehaviour
{
    #region Instance Variables
    [SerializeField]
    [Tooltip("The amount to rotate left (angles ) the tunnel")]
    private float rotationAmountLeft = 90.0f;
    [SerializeField]
    [Tooltip("The amount to rotate right (angles negative) the tunnel")]
    private float rotationAmountRight = -90.0f;
    [SerializeField]
    [Tooltip("The amount of time it takes to rotate the tunnel (in seconds)")]
    private float timeToRotate = 1.0f;
    [SerializeField]
    [Tooltip("The ball object itself")]
    private GameObject ball = null;
    #endregion

    #region Hidden Variables
    private Vector3 rotationPoint = Vector3.zero;
    private bool isRotating = false;
    private PlayerCollision playerCollision = null;
    #endregion

    private void Start()
    {
        playerCollision = ball.GetComponent<PlayerCollision>();
    }

    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyDown(KeyCode.A) || Input.GetKeyDown(KeyCode.LeftArrow))
        {
            RotateLeft();
        }
        else if (Input.GetKeyDown(KeyCode.D) || Input.GetKeyDown(KeyCode.RightArrow))
        {
            RotateRight();
        }
    }

    public void RotateLeft()
    {
        if (CanRotate())
        {
            isRotating = true;
            StartCoroutine(RotateTunnelOverTime(timeToRotate, rotationAmountLeft));
        }
    }

    public void RotateRight()
    {
        if (CanRotate())
        {
            isRotating = true;
            StartCoroutine(RotateTunnelOverTime(timeToRotate, rotationAmountRight));
        }
    }

    private bool CanRotate()
    {
        if (playerCollision != null && !isRotating && playerCollision.CheckOnGround())
            return true;
        return false;
    }

    private IEnumerator RotateTunnelOverTime(float timeToRotate, float rotateAmount)
    {
        if(ball != null)
        {
            float currentRotation = this.gameObject.transform.eulerAngles.z;
            float targetRotation = currentRotation + rotateAmount;

            float elapsedTime = 0;
            if(ball != null)
                ball.GetComponent<Rigidbody>().isKinematic = true;
            while (elapsedTime < timeToRotate)
            {
                elapsedTime += Time.deltaTime;
                float zRotationAmount = Mathf.Lerp(currentRotation, targetRotation, elapsedTime / timeToRotate);
                this.gameObject.transform.RotateAround(rotationPoint, Vector3.forward, zRotationAmount - this.gameObject.transform.eulerAngles.z);

                yield return null;
            }
            isRotating = false;
            if(ball != null)
                ball.GetComponent<Rigidbody>().isKinematic = false;

            playerCollision.CheckForPitDeath();
        }
     
    }
}