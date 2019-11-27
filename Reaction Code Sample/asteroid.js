const sizes = {
    LARGE: 2,
    MEDIUM: 1,
    SMALL: 0
}

var Asteroid = new Phaser.Class({
    Extends: Phaser.GameObjects.Sprite,

    initialize:

    function Asteroid(scene, posX, posY, size) {
        Phaser.GameObjects.Sprite.call(this, scene)

        this.isChild = false;
        this.selectedTexture = 1;

        this.size = size;

        // Change the scale and speed of the asteroids to match the size of the asteroid.
        // Smaller asteroids vary more in speed and can move faster.
        switch (size) {
            case sizes.LARGE:
                this.selectTexture(Math.random());

                this.speed = 0.05;
                this.angle = 360 * Math.random();
                this.spinSpeed = 0.025 * Math.random() - 0.0125;
                break;
            case sizes.MEDIUM:
                this.selectTexture(Math.random());

                this.speed = 0.05 + 0.05 * Math.random();
                this.angle = 360 * Math.random();
                this.spinSpeed = 0.2 * Math.random() - 0.1;
                break;
            case sizes.SMALL:
                this.setTexture('asteroid_small');

                this.speed = 0.05 + 0.15 * Math.random();
                this.angle = 360 * Math.random();
                this.spinSpeed = 0.8 * Math.random() - 0.4;
                break;
        }

        this.childAsteroids = [];
        // Randomly place the asteroid at the border of the screen
        if (posX == -1 && posY == -1) {
            posX = scene.origin.x;
            posY = scene.origin.y;
            var perimeter = 2 * (gameSettings.screenWidth - this.displayWidth) + 2 * 
            (gameSettings.screenHeight - this.displayHeight);
            var placement = perimeter * Math.random();
            if (placement < 2 * (gameSettings.screenWidth - this.displayWidth)) {
                if (placement < (gameSettings.screenWidth - this.displayWidth)) {
                    posX += placement + this.displayWidth / 2;
                    posY += this.displayHeight / 2;
                } else {
                    posX += placement - (gameSettings.screenWidth - this.displayWidth / 2);
                    posY += (gameSettings.screenHeight - this.displayHeight / 2);
                }
            } else {
                placement -= 2 * (gameSettings.screenHeight - this.displayHeight);
                if (placement < (gameSettings.screenHeight - this.displayHeight)) {
                    posX += this.displayWidth / 2;
                    posY += placement + this.displayHeight / 2;
                } else {
                    posX += (gameSettings.screenWidth - this.displayWidth / 2);
                    posY += placement - (gameSettings.screenHeight - this.displayHeight / 2);
                }
            }
        }

        this.setPosition(posX, posY);
        this.setOrigin(0.5);

        // Randomize the direction of the asteroid
        var dirX = Math.random() - 0.5;
        var dirY = Math.random() - 0.5;
        this.direction = new Phaser.Math.Vector2(dirX, dirY);
        this.direction.normalize();
    },

    move: function (delta, object) {
        if (this.isChild || !this.active)
            return;

        for (var i = 0; i < this.childAsteroids.length; i++) {
            if (!this.childAsteroids[i].active) {
                this.childAsteroids[i].setIsChild(false);
                for (var j = 0; j < this.childAsteroids.length; j++) {
                    if (i == j)
                        continue;

                    object.scene.asteroids.killAndHide(this.childAsteroids[j]);
                    object.scene.asteroids.remove(this.childAsteroids[j], true, true);
                }

                object.scene.asteroids.killAndHide(this);
                object.scene.asteroids.remove(this, true, true);
                return;
            }
        }

        // Move the asteroid based on the time passed. Wrap around when reaching the screen edge.
        var x = Phaser.Math.Wrap(this.x + delta * this.speed * this.direction.x,
            object.scene.origin.x, object.scene.origin.x + gameSettings.screenWidth);
        var y = Phaser.Math.Wrap(this.y + delta * this.speed * this.direction.y,
            object.scene.origin.y, object.scene.origin.y + gameSettings.screenHeight);
        this.setPosition(x, y);

        this.angle += delta * this.spinSpeed;

        // Add "child asteroids" to this asteroid. To the player it just looks like one asteroid
        // wrapping around the screen.
        var childrenCount = 0;

        if (x >= object.scene.origin.x && x < object.scene.origin.x + this.displayWidth / 2) {
            childrenCount++;
            if (this.childAsteroids.length < childrenCount) {
                var newAsteroid = new Asteroid(object.scene, this.x + gameSettings.screenWidth, this.y, this.size);
                newAsteroid.setIsChild(true);
                this.childAsteroids.push(newAsteroid);
                object.scene.asteroids.add(newAsteroid, true);
            } else {
                this.childAsteroids[childrenCount - 1].setPosition(this.x + gameSettings.screenWidth, this.y);
            }
        } else if (x < object.scene.origin.x + gameSettings.screenWidth && 
            x > object.scene.origin.x + gameSettings.screenWidth - this.displayWidth / 2) {
            childrenCount++;
            if (this.childAsteroids.length < childrenCount) {
                var newAsteroid = new Asteroid(object.scene, this.x - gameSettings.screenWidth, this.y, this.size);
                newAsteroid.setIsChild(true);
                this.childAsteroids.push(newAsteroid);
                object.scene.asteroids.add(newAsteroid, true);
            } else {
                this.childAsteroids[childrenCount - 1].setPosition(this.x - gameSettings.screenWidth, this.y);
            }
        }

        if (y >= object.scene.origin.y && y < object.scene.origin.y + this.displayHeight / 2) {
            childrenCount++;
            if (this.childAsteroids.length < childrenCount) {
                var newAsteroid = new Asteroid(object.scene, this.x, this.y + gameSettings.screenHeight, this.size);
                newAsteroid.setIsChild(true);
                this.childAsteroids.push(newAsteroid);
                object.scene.asteroids.add(newAsteroid, true);
            } else {
                this.childAsteroids[childrenCount - 1].setPosition(this.x, this.y + gameSettings.screenHeight);
            }
        } else if (y < object.scene.origin.y + gameSettings.screenHeight && 
            y > object.scene.origin.y + gameSettings.screenHeight - this.displayHeight / 2) {
            childrenCount++;
            if (this.childAsteroids.length < childrenCount) {
                var newAsteroid = new Asteroid(object.scene, this.x, this.y - gameSettings.screenHeight, this.size);
                newAsteroid.setIsChild(true);
                this.childAsteroids.push(newAsteroid);
                object.scene.asteroids.add(newAsteroid, true);
            } else {
                this.childAsteroids[childrenCount - 1].setPosition(this.x, this.y - gameSettings.screenHeight);
            }
        }

        // Handle literal corner case
        if (childrenCount == 2) {
            childrenCount++;
            var cornerX = x;
            var cornerY = y;

            if (x < object.scene.origin.x + (gameSettings.screenWidth / 2)) {
                cornerX += gameSettings.screenWidth;
            } else {
                cornerX -= gameSettings.screenWidth;
            }

            if (y < object.scene.origin.y + (gameSettings.screenHeight / 2)) {
                cornerY += gameSettings.screenHeight;
            } else {
                cornerY -= gameSettings.screenHeight;
            }

            if (this.childAsteroids.length < childrenCount) {
                var newAsteroid = new Asteroid(object.scene, cornerX, cornerY, this.size);
                newAsteroid.setIsChild(true);
                this.childAsteroids.push(newAsteroid);
                object.scene.asteroids.add(newAsteroid, true);
            } else {
                this.childAsteroids[childrenCount - 1].setPosition(cornerX, cornerY);
            }
        }

        // Remove any "excess" asteroids
        while (this.childAsteroids.length > childrenCount) {
            object.scene.asteroids.killAndHide(this.childAsteroids[this.childAsteroids.length - 1]);
            object.scene.asteroids.remove(this.childAsteroids[this.childAsteroids.length - 1], true, true);
            this.childAsteroids.pop();
        }

        // Sync child rotation with parent
        for (var i = 0; i < this.childAsteroids.length; i++) {
            this.childAsteroids[i].angle = this.angle;
            this.childAsteroids[i].spinSpeed = this.spinSpeed;
            this.childAsteroids[i].selectTexture(this.selectedTexture);
        }
    },

    demolish: function (object, asteroids) {
        // If the asteroid isn't the smallest size, create 2 new asteroids in its location
        if (this.size > sizes.SMALL) {
            for (i = 0; i < gameSettings.asteroidBreakNum; i++) {
                asteroids.add(new Asteroid(object.scene, this.x, this.y, this.size - 1), true);
            }
        }
        object.scene.explodeAudio.play();
        // Kill the current asteroid
        asteroids.killAndHide(this);
        for (var j = 0; j < this.childAsteroids.length; j++) {
            object.scene.asteroids.killAndHide(this.childAsteroids[j]);
            object.scene.asteroids.remove(this.childAsteroids[j], true, true);
        }

        switch (this.size) {
            case sizes.LARGE:
                object.scene.score += 20;
                break;
            case sizes.MEDIUM:
                object.scene.score += 50;
                break;
            case sizes.SMALL:
                object.scene.score += 100;
                break;
              default:
                break;
        }

        var scoreFormatted = this.scene.zeroPad(this.scene.score, 6);
        object.scene.scoreLabel.text = "SCORE " + scoreFormatted;
        globalScore = scoreFormatted;
    },

    setIsChild: function (isChild) {
        this.isChild = isChild;
    },

    selectTexture: function (num) {
        if (this.selectedTexture == num)
            return;

        this.selectedTexture = num;

        switch (this.size) {
            case sizes.LARGE:
                if (num < 1 / 3)
                    this.setTexture('asteroid_big_1');
                else if (num < 2 / 3)
                    this.setTexture('asteroid_big_2');
                else
                    this.setTexture('asteroid_big_3');

                break;
            case sizes.MEDIUM:
                if (num < 1 / 2)
                    this.setTexture('asteroid_medium_1');
                else
                    this.setTexture('asteroid_medium_2');

                break;
            case sizes.SMALL:
                this.setTexture('asteroid_small');
                break;
        }
    }
});
